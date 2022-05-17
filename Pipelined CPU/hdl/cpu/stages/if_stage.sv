import rv32i_types::*;

module if_stage(
    /* inputs */
    input clk,
    input rst,
    input logic stall_n,

    //input rv32i_word alu_out,
    input pcmux::pcmux_sel_t pc_mux_sel,
    input rv32i_word pc_mux_out,

    input logic nop_en,
    input logic br_flush,

    input rv32i_word bht_out,
    input logic pred_hit,

    /* mem inputs */
    input logic [31:0] i_mem_rdata,
    input logic i_mem_resp,
    input logic [2:0] take_lgp,

    /* outputs */
    output rv32i_reg rs1,
    output rv32i_reg rs2,
    output rv32i_reg rd,

    output rv32i_word i_imm,
    output rv32i_word u_imm,
    output rv32i_word b_imm,
    output rv32i_word s_imm,
    output rv32i_word j_imm,

    output rv32i_opcode opcode,
    output logic [2:0] funct3,
    output logic [6:0] funct7,

    output rv32i_word pc2_out, //this pc gets passed on through stages
    output rv32i_word pc_pred_out,

    /* rvfi output */
    // synthesis translate_off
    output rvfi_mon_word rvfi_word_out,
    // synthesis translate_on

    output logic [2:0] take_lgp_out,

    /* mem outputs */
    output logic [31:0] i_mem_addr,
    output logic i_mem_read

    
);

//rv32i_word pc_mux_out; //chose between PC + 4 and branch pc
rv32i_word pc_pred; //predicted PC from BTB
rv32i_word pc_out; //input to PC2 reg
rv32i_word pc_in; //selects between actual pc and predicted pc

logic jal_sel, rd_link, rs1_link, push, pop;
logic [2:0] take_lgp_pc;


assign i_mem_read = (!nop_en && !br_flush);
assign i_mem_addr = {pc_out[31:2], 2'b0};

assign pc_pred_out = pc_out;

assign pc_pred = pred_hit ? bht_out : pc_out + 4; // Prediction
//assign pc_in = br_flush ? pc_mux_out : pc_pred; // Decision

//RAS
rv32i_opcode ras_opcode;
rv32i_word ras_j_imm, data;

assign data = i_mem_rdata;

assign ras_opcode = rv32i_opcode'(data[6:0]);
assign ras_j_imm = {{12{data[31]}}, data[19:12], data[20], data[30:21], 1'b0};

assign jal_sel = (i_mem_resp && ras_opcode == op_jal);

always_comb begin : RAS
    if (br_flush)
        pc_in = pc_mux_out;
    else if (jal_sel) begin
         pc_in = pc_out + ras_j_imm;
    end
    else 
        pc_in = pc_pred;
end

// Instruction Register
ir IR(
    .clk(clk),
    .rst((rst || br_flush)),
    
    .load((stall_n && !nop_en)),
    .in(i_mem_rdata),

    .funct3(funct3),
    .funct7(funct7),
    .opcode(opcode),
    .i_imm(i_imm),
    .s_imm(s_imm),
    .b_imm(b_imm),
    .u_imm(u_imm),
    .j_imm(j_imm),
    .rs1(rs1),
    .rs2(rs2),
    .rd(rd)
);

// PC
pc_register PC(
    .clk(clk),
    .rst(rst),
    .load((stall_n && !nop_en)),
    .in(pc_in),
    .out(pc_out)
);

// PC2 (this gets passed on)
pc_register PC2(
    .clk(clk),
    .rst(rst),
    .load((stall_n && !nop_en)),
    .in(pc_out),
    .out(pc2_out)
);

register #(.width(3)) take_lg_reg(
    .clk(clk),
    .rst(rst),
    .load((stall_n && !nop_en)),
    .in(take_lgp),
    .out(take_lgp_pc)
);

register #(.width(3)) take_lg_reg_2(
    .clk(clk),
    .rst(rst),
    .load((stall_n && !nop_en)),
    .in(take_lgp_pc),
    .out(take_lgp_out)
);

// synthesis translate_off

rvfi_mon_word rvfi_word;

rvfi_register rvfi_reg(
    .clk(clk),
    .rst(rst),
    .load((stall_n && !nop_en)),
    .in(rvfi_word),
    .out(rvfi_word_out)
);


always_comb
begin
    rvfi_word = '0;
    rvfi_word.inst = i_mem_rdata;
    rvfi_word.pc_rdata = pc_out;
    rvfi_word.pc_wdata = pc_in;
end

// synthesis translate_on

endmodule : if_stage