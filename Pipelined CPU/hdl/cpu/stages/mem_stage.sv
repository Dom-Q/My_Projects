import rv32i_types::*;
`define BAD_MUX_SEL $fatal("%0t %s %0d: Illegal mux select", $time, `__FILE__, `__LINE__)
module mem_stage(

    /* inputs */
    input clk,
    input rst,
    input rv32i_control_word ctrl_in,
    input logic cmp_in,
    input rv32i_reg rd_in,
    input rv32i_word rs2_in,
    input rv32i_word alu_in,
    input rv32i_word u_imm,
    input rv32i_word pc_out,
    input logic stall_n,

    /* mem input signals */
    input logic [31:0] d_mem_rdata,
    input logic d_mem_resp,
    
    /* outputs */
    output pcmux::pcmux_sel_t pcmux_sel,
    output rv32i_word alu_out,
    output rv32i_reg rd_out,
    output rv32i_control_word ctrl_out,
    output rv32i_word mem_rdata_out,
    output logic cmp_out,
    output rv32i_word u_imm_reg_out,
    output rv32i_word pc_reg_out,
    output logic br_flush,
    output rv32i_word pc_mux_out,

    // synthesis translate_off
    /* rvfi input output */
    input rvfi_mon_word rvfi_word,
    output rvfi_mon_word rvfi_word_out,
    // synthesis translate_on

    /* mem output signals */
    output logic [31:0] d_mem_addr,
    output logic d_mem_read,
    output logic [31:0] d_mem_wdata,
    output logic [3:0] d_mem_byte_enable,
    output logic d_mem_write,
    output rv32i_word forward_mem
);

logic [1:0] addr_idx, addr_idx_2;
logic br_sig, pc_sel;
store_funct3_t store_funct3;
logic [15:0] mdr_half;
logic [7:0] mdr_byte;

assign br_sig = cmp_in || ctrl_in.br_jal;
assign pc_sel = ctrl_in.br_en && br_sig;

assign pcmux_sel = pcmux::pcmux_sel_t'(pc_sel);
assign pc_mux_out = pc_sel ? alu_in : pc_out + 4;
//assign br_flush = (pc_mux_out != ctrl_in.pc_pred) && (ctrl_in.br_en || ctrl_in.br_jal);
assign br_flush = (ctrl_in.opcode != '0) ? (pc_mux_out != ctrl_in.pc_pred) : 1'b0;

assign addr_idx = alu_in[1:0];
assign store_funct3 = store_funct3_t'(ctrl_in.funct3);

assign d_mem_read = ctrl_in.mem_read;
assign d_mem_addr = {alu_in[31:2], 2'b0};
assign d_mem_write = ctrl_in.mem_write;
assign d_mem_wdata = rs2_in << (8*addr_idx);

assign addr_idx_2 = alu_out[1:0];
assign mdr_byte = mem_rdata_out[8*addr_idx_2 +: 8];

// Stage Registers
ctrl_register ctrl_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(ctrl_in),
    .out(ctrl_out)
);

register alu_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(alu_in),
    .out(alu_out)
);

register #(.width(5)) rd_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(rd_in),
    .out(rd_out)
);

register mem_rdata_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(d_mem_rdata),
    .out(mem_rdata_out)
);

register #(.width(1)) br_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(cmp_in),
    .out(cmp_out)
);

register u_imm_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(u_imm),
    .out(u_imm_reg_out)
);

pc_register PC(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(pc_out),
    .out(pc_reg_out)
);

always_comb begin
    unique case (addr_idx_2)
        2'b00: mdr_half = mem_rdata_out[15 : 0];
        2'b10: mdr_half = mem_rdata_out[31 : 16];
        default : mdr_half = 16'b0;
    endcase
end

always_comb begin

    unique case (ctrl_out.regfilemux_sel)
        regfilemux::alu_out: forward_mem = alu_out;
        regfilemux::br_en: forward_mem = cmp_out;
        regfilemux::u_imm: forward_mem = u_imm_reg_out;
        regfilemux::lw: forward_mem = mem_rdata_out;
        regfilemux::lb: forward_mem = {{24{mdr_byte[7]}}, mdr_byte};
        regfilemux::lbu: forward_mem = {24'b0, mdr_byte};
        regfilemux::lh: forward_mem = {{16{mdr_half[15]}}, mdr_half};
        regfilemux::lhu: forward_mem = {16'b0, mdr_half};
        regfilemux::pc_plus4: forward_mem = pc_reg_out + 4;
        default: `BAD_MUX_SEL;
    endcase    

    unique case (store_funct3)
        sb: d_mem_byte_enable = 4'b0001 << addr_idx;
        sh: d_mem_byte_enable = 4'b0011 << addr_idx;
        default: d_mem_byte_enable = 4'b1111;
    endcase
end

// synthesis translate_off
rvfi_mon_word rvfi_word_local;

rvfi_register rvfi_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(rvfi_word_local),
    .out(rvfi_word_out)
);

logic trap;
logic [4:0] rs1_addr, rs2_addr;
logic [3:0] rmask, wmask;


branch_funct3_t branch_funct3;
load_funct3_t load_funct3;

assign branch_funct3 = branch_funct3_t'(ctrl_in.funct3);
assign load_funct3 = load_funct3_t'(ctrl_in.funct3);

always_comb begin : trap_check

    rvfi_word_local = rvfi_word;
    rvfi_word_local.mem_addr = d_mem_addr;
    rvfi_word_local.mem_rdata = d_mem_rdata;
    rvfi_word_local.mem_wdata = d_mem_wdata;

    rvfi_word_local.pc_wdata  = pc_mux_out;

    if (rvfi_word_local.pc_rdata == rvfi_word_local.pc_wdata) //TODO (check if rvfi.pc_rdata or rvfi.pc_wdata)
        rvfi_word_local.halt = 1;
    

    // unique case (pcmux_sel)
    //     pcmux::alu_out : begin
    //         if (rvfi_word_local.pc_rdata == pc_wdata) //TODO (check if rvfi.pc_rdata or rvfi.pc_wdata)
    //             rvfi_word_local.halt = 1;
    //     end
    //     default: ;
    // endcase

    case (ctrl_in.opcode)
        op_lui, op_auipc, op_imm, op_reg, op_jal, op_jalr:;

        op_br: begin
            case (branch_funct3)
                beq, bne, blt, bge, bltu, bgeu:;
                default: rvfi_word_local.trap = 1;
            endcase
        end

        op_load: begin
            case (load_funct3)
                lw: rvfi_word_local.mem_rmask = 4'b1111;
                lh, lhu: rvfi_word_local.mem_rmask = 4'b0011 << addr_idx /* Modify for MP1 Final */ ;
                lb, lbu: rvfi_word_local.mem_rmask = 4'b0001 << addr_idx /* Modify for MP1 Final */ ;
                default: rvfi_word_local.trap = 1;
            endcase
        end

        op_store: begin
            case (store_funct3)
                sw: rvfi_word_local.mem_wmask = 4'b1111;
                sh: rvfi_word_local.mem_wmask = 4'b0011 << addr_idx /* Modify for MP1 Final */ ;
                sb: rvfi_word_local.mem_wmask = 4'b0001 << addr_idx /* Modify for MP1 Final */ ;
                default: rvfi_word_local.trap = 1;
            endcase
        end

        default: rvfi_word_local.trap = 1;
    endcase
end
// synthesis translate_on

endmodule