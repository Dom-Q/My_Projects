package stage_ctrl;

import rv32i_types::*;
import pcmux::*;
import marmux::*;
import cmpmux::*;
import alumux::*;
import regfilemux::*;

typedef struct packed
{
    /* inputs */
    //rv32i_word alu_out;
    pcmux::pcmux_sel_t pc_mux_sel;
    rv32i_word pc_mux_out;

    /* outputs */
    rv32i_reg rs1;
    rv32i_reg rs2;
    rv32i_reg rd;

    rv32i_word i_imm;
    rv32i_word u_imm;
    rv32i_word b_imm;
    rv32i_word s_imm;
    rv32i_word j_imm;

    rv32i_opcode opcode;
    logic [2:0] funct3;
    logic [6:0] funct7;

    rv32i_word pc2_out;
    rv32i_word pc_pred;
    
    rvfi_mon_word rvfi_word_out;
} stage_if;

typedef struct packed
{
    /* inputs */
    rv32i_reg rs1;
    rv32i_reg rs2;
    rv32i_reg rd;
    rv32i_reg rd_reg_in;
    rv32i_word regfilemux_out;
    logic load_regfile;

    rv32i_word i_imm;
    rv32i_word u_imm;
    rv32i_word b_imm;
    rv32i_word s_imm;
    rv32i_word j_imm;

    rv32i_opcode opcode;
    logic [2:0] funct3;
    logic [6:0] funct7;

    rv32i_word pc_out;
    rv32i_word pc_pred;

    /* outputs */
    rv32i_word pc_reg_out;

    rv32i_word rs1_out;
    rv32i_word rs2_out;
    rv32i_reg rd_reg_out;
    
    rv32i_word i_imm_reg_out;
    rv32i_word u_imm_reg_out;
    rv32i_word b_imm_reg_out;
    rv32i_word s_imm_reg_out;
    rv32i_word j_imm_reg_out;

    rv32i_control_word ctrl_out;

    rvfi_mon_word rvfi_word;
    rvfi_mon_word rvfi_word_out;
} stage_id;

typedef struct packed
{
    /* inputs */
    rv32i_reg rd;
    rv32i_word rs1_out;
    rv32i_word pc_out;
    rv32i_word rs2;
    rv32i_control_word ctrl_in;
    rv32i_word i_imm;
    rv32i_word u_imm;
    rv32i_word b_imm;
    rv32i_word s_imm;
    rv32i_word j_imm;

    /* outputs */
    logic cmp_out;
    rv32i_reg rd_out;
    rv32i_word rs2_out;
    rv32i_word alu_out;
    rv32i_control_word ctrl_out;
    rv32i_word u_imm_reg_out;
    rv32i_word pc_reg_out;
    rv32i_word forward_ex;
    rvfi_mon_word rvfi_word;
    rvfi_mon_word rvfi_word_out;
} stage_ex;

typedef struct packed
{
    /* inputs */
    rv32i_control_word ctrl_in;
    logic cmp_in;
    rv32i_reg rd_in;
    rv32i_word rs2_in;
    rv32i_word alu_in;
    rv32i_word u_imm;
    rv32i_word pc_out;

    /* outputs */
    pcmux::pcmux_sel_t pcmux_sel;
    rv32i_word alu_out;
    rv32i_reg rd_out;
    rv32i_control_word ctrl_out;
    rv32i_word mem_rdata_out;
    logic cmp_out;
    rv32i_word u_imm_reg_out;
    rv32i_word pc_reg_out;
    rv32i_word forward_mem;
    rvfi_mon_word rvfi_word;
    rvfi_mon_word rvfi_word_out;
    rv32i_word pc_mux_out;
} stage_mem;

typedef struct packed
{
    /* inputs */
    rv32i_word mem_rdata_in;
    rv32i_control_word ctrl_in;
    rv32i_word alu_out;
    logic cmp_out;
    rv32i_word u_imm;
    rv32i_word pc_out;

    /* output */
    rv32i_word regfilemux_out;

    rvfi_mon_word rvfi_word;
} stage_wb;

endpackage: stage_ctrl