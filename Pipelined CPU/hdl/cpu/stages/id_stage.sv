`define BAD_MUX_SEL $fatal("%0t %s %0d: Illegal mux select", $time, `__FILE__, `__LINE__)

import rv32i_types::*;

module id_stage
(
    /* inputs */
    input clk,
    input rst,
    input logic stall_n,
    
    input rv32i_reg rs1,
    input rv32i_reg rs2,
    input rv32i_reg rd,
    input rv32i_reg rd_reg_in,
    input logic load_regfile,
    input rv32i_word regfilemux_out,

    input rv32i_word i_imm,
    input rv32i_word u_imm,
    input rv32i_word b_imm,
    input rv32i_word s_imm,
    input rv32i_word j_imm,

    input rv32i_opcode opcode,
    input logic [2:0] funct3,
    input logic [6:0] funct7,

    input rv32i_word pc_out,

    input logic nop_en,
    input logic br_flush,

    input rv32i_word pc_pred,
    input logic [2:0] take_lgp,

    /* outputs */
    output rv32i_word pc_reg_out,

    output rv32i_word rs1_out,
    output rv32i_word rs2_out,
    output rv32i_reg rd_reg_out,

    // synthesis translate_off
    /* rvfi input output */
    input rvfi_mon_word rvfi_word,
    output rvfi_mon_word rvfi_word_out,
    // synthesis translate_on
    
    output rv32i_word i_imm_reg_out,
    output rv32i_word u_imm_reg_out,
    output rv32i_word b_imm_reg_out,
    output rv32i_word s_imm_reg_out,
    output rv32i_word j_imm_reg_out,

    output rv32i_control_word ctrl_out
);

rv32i_word temp_rs1;
rv32i_word temp_rs2;
rv32i_control_word ctrl_in, ctrl_rom_out;

assign ctrl_in = (nop_en || br_flush) ? '0 : ctrl_rom_out;

/* registers */
pc_register PC(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(pc_out),
    .out(pc_reg_out)
);

/* regfile and rs/rd registers */
regfile regFile(
    .clk(clk),
    .rst(rst),
    .load(load_regfile & stall_n),
    .in(regfilemux_out),
    .src_a(rs1),
    .src_b(rs2),
    .dest(rd),
    .reg_a(temp_rs1),
    .reg_b(temp_rs2)
);

register rs1_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(temp_rs1),
    .out(rs1_out)
);

register rs2_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(temp_rs2),
    .out(rs2_out)
);

register #(.width(5)) rd_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(rd_reg_in),
    .out(rd_reg_out)
);

/* control rom */
control_rom controlRom(
    .opcode(opcode),
    .funct3(funct3),
    .funct7(funct7),
    .pc_pred(pc_pred),
    .rs1(rs1),
    .rs2(rs2),
    .take_lgp(take_lgp),
    .ctrl(ctrl_rom_out)
);

/* imm_* registers */
register i_imm_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(i_imm),
    .out(i_imm_reg_out)
);

register u_imm_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(u_imm),
    .out(u_imm_reg_out)
);

register b_imm_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(b_imm),
    .out(b_imm_reg_out)
);

register s_imm_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(s_imm),
    .out(s_imm_reg_out)
);

register j_imm_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(j_imm),
    .out(j_imm_reg_out)
);

/* wb, ex, and mem register combined as a ctrl reg */
ctrl_register ctrl_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(ctrl_in),
    .out(ctrl_out)
);

// synthesis translate_off
rvfi_mon_word rvfi_word_local;

always_comb
begin
    rvfi_word_local = rvfi_word;
    rvfi_word_local.rs1_addr = rs1;
    rvfi_word_local.rs2_addr = rs2;
    rvfi_word_local.load_regfile = ctrl_in.load_regfile;
    rvfi_word_local.rd_addr = rd_reg_in;
end

rvfi_register rvfi_reg(
    .clk(clk),
    .rst(rst),
    .load(stall_n),
    .in(rvfi_word_local),
    .out(rvfi_word_out)
);
// synthesis translate_on

endmodule : id_stage