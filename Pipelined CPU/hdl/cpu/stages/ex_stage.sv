`define BAD_MUX_SEL $fatal("%0t %s %0d: Illegal mux select", $time, `__FILE__, `__LINE__)

import rv32i_types::*;

module ex_stage
(
    /* inputs */
    input clk,
    input rst,
    input logic stall_n,
    input rv32i_reg rd,
    input rv32i_word rs1_out,
    input rv32i_word pc_out,
    input rv32i_word rs2,
    input rv32i_control_word ctrl_in,
    input rv32i_word i_imm,
    input rv32i_word u_imm,
    input rv32i_word b_imm,
    input rv32i_word s_imm,
    input rv32i_word j_imm,
    input logic br_flush,
    input logic mem_stall,

    // synthesis translate_off
    input rvfi_mon_word rvfi_word,
    output rvfi_mon_word rvfi_word_out,
    // synthesis translate_on
    
    /* outputs */
    output logic cmp_out,
    output rv32i_reg rd_out,
    output rv32i_word rs2_out,
    output rv32i_word alu_out,
    output rv32i_control_word ctrl_out,
    output rv32i_word u_imm_reg_out,
    output rv32i_word pc_reg_out,
    output rv32i_word forward_ex,
    output logic muldiv_stall
);

alu_ops aluop;
branch_funct3_t cmpop;
rv32i_word alumux1_out, alumux2_out, cmpmux_out, alu_op_out, muldiv_out, alu_reg_in;
rv32i_control_word ctrl_reg_in;
logic br_en, muldiv_resp;

assign aluop = ctrl_in.aluop;
assign cmpop = ctrl_in.cmpop;
assign ctrl_reg_in = br_flush ? '0 : ctrl_in;
assign alu_reg_in = ctrl_in.muldiv_en ? muldiv_out : alu_op_out;
assign muldiv_stall = (ctrl_in.muldiv_en & !muldiv_resp);

// ALU and CMP
alu ALU (
    .aluop(aluop),
    .a(alumux1_out),
    .b(alumux2_out),
    .f(alu_op_out)
);

muldiv MULDIV
(
    .clk(clk),
    .rst(rst),
    .a(rs1_out),
    .b(rs2),
    .muldiv_en(ctrl_in.muldiv_en),
    .funct3(muldiv_funct3_t'(ctrl_in.funct3)),
    .mem_stall(mem_stall),
    .f(muldiv_out),
    .muldiv_resp(muldiv_resp)
);

cmp CMP (
    .cmpop(cmpop),
    .a(rs1_out),
    .b(cmpmux_out),
    .f(br_en)
);

// Stage Registers
ctrl_register ctrl_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(ctrl_reg_in),
    .out(ctrl_out)
); 

register #(.width(1)) cmp_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(br_en),
    .out(cmp_out)
);

register #(.width(5)) rd_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(rd),
    .out(rd_out)
);

register rs2_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(rs2),
    .out(rs2_out)
);

register alu_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(alu_reg_in),
    .out(alu_out)
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


always_comb begin : MUXES
    unique case (ctrl_out.regfilemux_sel)
        regfilemux::alu_out: forward_ex= alu_out;
        regfilemux::br_en: forward_ex = cmp_out;
        regfilemux::u_imm: forward_ex = u_imm_reg_out;
        regfilemux::pc_plus4: forward_ex = pc_reg_out + 4;
        default: begin
            forward_ex = 32'bx;
        end
    endcase    

    unique case (ctrl_in.alumux1_sel)
        alumux::rs1_out: alumux1_out = rs1_out;
        alumux::pc_out: alumux1_out = pc_out;
        default: `BAD_MUX_SEL;
    endcase

    unique case (ctrl_in.alumux2_sel)
        alumux::i_imm: alumux2_out = i_imm;
        alumux::s_imm: alumux2_out = s_imm;
        alumux::b_imm: alumux2_out = b_imm;
        alumux::u_imm: alumux2_out = u_imm;
        alumux::j_imm: alumux2_out = j_imm;
        alumux::rs2_out: alumux2_out = rs2;
        default: `BAD_MUX_SEL;
    endcase

    unique case (ctrl_in.cmpmux_sel)
        cmpmux::rs2_out: cmpmux_out = rs2;
        cmpmux::i_imm: cmpmux_out = i_imm;
        default: `BAD_MUX_SEL;
    endcase
end : MUXES

// synthesis translate_off
rvfi_mon_word rvfi_word_local;

always_comb begin
    rvfi_word_local = rvfi_word;
    rvfi_word_local.rs1_rdata = rs1_out;
    rvfi_word_local.rs2_rdata = rs2;
end

rvfi_register rvfi_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(rvfi_word_local),
    .out(rvfi_word_out)
);
// synthesis translate_on

endmodule : ex_stage