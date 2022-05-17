`define BAD_MUX_SEL $fatal("%0t %s %0d: Illegal mux select", $time, `__FILE__, `__LINE__)

import rv32i_types::*;
import stage_ctrl::*;

module datapath
(
    input clk,
    input rst,

    input [31:0] i_mem_rdata,
    input logic i_mem_resp,
    input [31:0] d_mem_rdata,
    input logic d_mem_resp,

    output logic [31:0] i_mem_addr,
    output logic i_mem_read,
    
    output logic d_mem_read,
    output logic d_mem_write,
    output logic [31:0] d_mem_addr,
    output logic [31:0] d_mem_wdata,
    output logic [3:0] d_mem_byte_enable
    
);


logic stall_n;
logic nop_en;
logic br_flush;
logic pred_hit;
logic [2:0] take_lgp, take_lgp_out;
logic i_mem_stall, d_mem_stall, mem_stall, muldiv_stall;
rv32i_word bht_out;

assign i_mem_stall = (i_mem_read && !i_mem_resp);
assign d_mem_stall = ((d_mem_read ^ d_mem_write) && !d_mem_resp);
assign mem_stall = (i_mem_stall || d_mem_stall);
assign stall_n = !(mem_stall || muldiv_stall);

/* stage vars */
stage_ctrl::stage_if stageIF;
stage_ctrl::stage_id stageID;
stage_ctrl::stage_ex stageEX;
stage_ctrl::stage_mem stageMEM;
stage_ctrl::stage_wb stageWB;

/* forwarding muxes sel vars */
forwardmux::forward_sel_t forwardA;
forwardmux::forward_sel_t forwardB;

/////////////////////* stage connector assignments */////////////////////

/* IF */
//assign stageIF.alu_out = stageEX.alu_out;
assign stageIF.pc_mux_sel = stageMEM.pcmux_sel;
assign stageIF.pc_mux_out = stageMEM.pc_mux_out; //golden PC

/* ID */
assign stageID.pc_pred = stageIF.pc_pred;
assign stageID.rs1 = stageIF.rs1;
assign stageID.rs2 = stageIF.rs2;
assign stageID.rd = stageMEM.rd_out;
assign stageID.rd_reg_in = stageIF.rd;
assign stageID.regfilemux_out = stageWB.regfilemux_out;
assign stageID.load_regfile = stageMEM.ctrl_out.load_regfile;
assign stageID.opcode = stageIF.opcode;
assign stageID.funct3 = stageIF.funct3;
assign stageID.funct7 = stageIF.funct7;
assign stageID.i_imm = stageIF.i_imm;
assign stageID.u_imm = stageIF.u_imm;
assign stageID.b_imm = stageIF.b_imm;
assign stageID.s_imm = stageIF.s_imm;
assign stageID.j_imm = stageIF.j_imm;
assign stageID.pc_out = stageIF.pc2_out;
// synthesis translate_off
assign stageID.rvfi_word = stageIF.rvfi_word_out;
// synthesis translate_on

/* EX */
assign stageEX.rd = stageID.rd_reg_out;
// assign stageEX.rs1_out = stageID.rs1_out;
// assign stageEX.rs2 = stageID.rs2_out;
assign stageEX.pc_out = stageID.pc_reg_out;
assign stageEX.ctrl_in = stageID.ctrl_out;
assign stageEX.i_imm = stageID.i_imm_reg_out;
assign stageEX.u_imm = stageID.u_imm_reg_out;
assign stageEX.b_imm = stageID.b_imm_reg_out;
assign stageEX.s_imm = stageID.s_imm_reg_out;
assign stageEX.j_imm = stageID.j_imm_reg_out;
// synthesis translate_off
assign stageEX.rvfi_word = stageID.rvfi_word_out;
// synthesis translate_on

/* MEM */
assign stageMEM.ctrl_in = stageEX.ctrl_out;
assign stageMEM.cmp_in = stageEX.cmp_out;
assign stageMEM.rd_in = stageEX.rd_out;
assign stageMEM.rs2_in = stageEX.rs2_out;
assign stageMEM.alu_in = stageEX.alu_out;
assign stageMEM.u_imm = stageEX.u_imm_reg_out;
assign stageMEM.pc_out = stageEX.pc_reg_out;
// synthesis translate_off
assign stageMEM.rvfi_word = stageEX.rvfi_word_out;
// synthesis translate_on
 
/* WB */
assign stageWB.mem_rdata_in = stageMEM.mem_rdata_out;
assign stageWB.ctrl_in = stageMEM.ctrl_out;
assign stageWB.alu_out = stageMEM.alu_out;
assign stageWB.cmp_out = stageMEM.cmp_out;
assign stageWB.u_imm = stageMEM.u_imm_reg_out;
assign stageWB.pc_out = stageMEM.pc_reg_out;
// synthesis translate_off
assign stageWB.rvfi_word = stageMEM.rvfi_word_out;
// synthesis translate_on

/////////////////////* end of stage connector assignments */////////////////////


// Stages
if_stage IF(
    .clk(clk),
    .rst(rst),
    //.alu_out(stageIF.alu_out),
    .pc_mux_sel(stageIF.pc_mux_sel),
    .pc_mux_out(stageIF.pc_mux_out),
    .i_mem_rdata(i_mem_rdata),
    .i_mem_resp(i_mem_resp),
    .stall_n(stall_n),
	.nop_en(nop_en),
    .rs1(stageIF.rs1),
    .rs2(stageIF.rs2),
    .rd(stageIF.rd),
    .br_flush(br_flush),
    .bht_out(bht_out),
    .pred_hit(pred_hit),
    .i_imm(stageIF.i_imm),
    .u_imm(stageIF.u_imm),
    .b_imm(stageIF.b_imm),
    .s_imm(stageIF.s_imm),
    .j_imm(stageIF.j_imm),
    .opcode(stageIF.opcode),
    .funct3(stageIF.funct3),
    .funct7(stageIF.funct7),
    .pc2_out(stageIF.pc2_out),
    .i_mem_addr(i_mem_addr),
    .i_mem_read(i_mem_read),
    .pc_pred_out(stageIF.pc_pred),
    // synthesis translate_off
    .rvfi_word_out(stageIF.rvfi_word_out),
    // synthesis translate_on
    .take_lgp(take_lgp),
    .take_lgp_out(take_lgp_out)
);

id_stage ID(
    .clk(clk),
    .rst(rst),
    .rs1(stageID.rs1),
    .rs2(stageID.rs2),
    .rd(stageID.rd),
    .rd_reg_in(stageID.rd_reg_in),
    .regfilemux_out(stageID.regfilemux_out),
    .load_regfile(stageID.load_regfile),
    .i_imm(stageID.i_imm),
    .u_imm(stageID.u_imm),
    .b_imm(stageID.b_imm),
    .s_imm(stageID.s_imm),
    .j_imm(stageID.j_imm),
    .opcode(stageID.opcode),
    .funct3(stageID.funct3),
    .funct7(stageID.funct7),
    .pc_out(stageID.pc_out),
    .stall_n(stall_n),
	.nop_en(nop_en),
    .br_flush(br_flush),
    .pc_pred(stageID.pc_pred),
    .take_lgp(take_lgp_out),

    // synthesis translate_off
    .rvfi_word(stageID.rvfi_word),
    .rvfi_word_out(stageID.rvfi_word_out),
    // synthesis translate_on
    
    .pc_reg_out(stageID.pc_reg_out),
    .rs1_out(stageID.rs1_out),
    .rs2_out(stageID.rs2_out),
    .rd_reg_out(stageID.rd_reg_out),
    .i_imm_reg_out(stageID.i_imm_reg_out),
    .u_imm_reg_out(stageID.u_imm_reg_out),
    .b_imm_reg_out(stageID.b_imm_reg_out),
    .s_imm_reg_out(stageID.s_imm_reg_out),
    .j_imm_reg_out(stageID.j_imm_reg_out),
    .ctrl_out(stageID.ctrl_out)
    
);

ex_stage EX(
    .clk(clk),
    .rst(rst),
    .rd(stageEX.rd),
    .rs1_out(stageEX.rs1_out),
    .pc_out(stageEX.pc_out),
    .rs2(stageEX.rs2),
    .ctrl_in(stageEX.ctrl_in),
    .i_imm(stageEX.i_imm),
    .u_imm(stageEX.u_imm),
    .b_imm(stageEX.b_imm),
    .s_imm(stageEX.s_imm),
    .j_imm(stageEX.j_imm),
    .mem_stall(mem_stall),
    .stall_n(stall_n),
    .br_flush(br_flush),
    .muldiv_stall(muldiv_stall),

    // synthesis translate_off
    .rvfi_word(stageEX.rvfi_word),
    .rvfi_word_out(stageEX.rvfi_word_out),
    // synthesis translate_on

    .cmp_out(stageEX.cmp_out),
    .rd_out(stageEX.rd_out),
    .rs2_out(stageEX.rs2_out),
    .alu_out(stageEX.alu_out),
    .ctrl_out(stageEX.ctrl_out),
    .u_imm_reg_out(stageEX.u_imm_reg_out),
    .pc_reg_out(stageEX.pc_reg_out),
    .forward_ex(stageEX.forward_ex)
);

mem_stage MEM(
    .clk(clk),
    .rst(rst),
    .ctrl_in(stageMEM.ctrl_in),
    .cmp_in(stageMEM.cmp_in),
    .rd_in(stageMEM.rd_in),
    .rs2_in(stageMEM.rs2_in),
    .alu_in(stageMEM.alu_in),
    .d_mem_rdata(d_mem_rdata),
    .u_imm(stageMEM.u_imm),
    .pc_out(stageMEM.pc_out),
    .stall_n(stall_n),
    .d_mem_resp(d_mem_resp),

    // synthesis translate_off
    .rvfi_word(stageMEM.rvfi_word),
    .rvfi_word_out(stageMEM.rvfi_word_out),
    // synthesis translate_on

    .pcmux_sel(stageMEM.pcmux_sel),
    .d_mem_addr(d_mem_addr),
    .d_mem_read(d_mem_read),
    .d_mem_wdata(d_mem_wdata),
    .d_mem_byte_enable(d_mem_byte_enable),
    .d_mem_write(d_mem_write),
    .alu_out(stageMEM.alu_out),
    .rd_out(stageMEM.rd_out),
    .ctrl_out(stageMEM.ctrl_out),
    .mem_rdata_out(stageMEM.mem_rdata_out),
    .cmp_out(stageMEM.cmp_out),
    .u_imm_reg_out(stageMEM.u_imm_reg_out),
    .pc_reg_out(stageMEM.pc_reg_out),
    .forward_mem(stageMEM.forward_mem),
    .br_flush(br_flush),
    .pc_mux_out(stageMEM.pc_mux_out)
);

wb_stage WB(
    .clk(clk),
    .rst(rst),
    .mem_rdata_in(stageWB.mem_rdata_in),
    .ctrl_in(stageWB.ctrl_in),
    .alu_out(stageWB.alu_out),
    .cmp_out(stageWB.cmp_out),

    // synthesis translate_off
    .rvfi_word(stageWB.rvfi_word),
    // synthesis translate_on

    .u_imm(stageWB.u_imm),
    .pc_out(stageWB.pc_out),
    .stall_n(stall_n),
    .regfilemux_out(stageWB.regfilemux_out)
);

branch_prediction #(.index(4), .btb_index(8), .N(4)) predictor
(
  .clk(clk),
  .rst(rst),
  .update_history((stageMEM.ctrl_in.br_en || stageMEM.ctrl_in.br_jal) && stall_n),
  .br_result(stageMEM.pcmux_sel),
  .if_pc(stageIF.pc_pred),
  .mem_pc(stageMEM.pc_out),
  .target_addr_in(stageMEM.pc_mux_out),
  .old_l(stageMEM.ctrl_in.old_l),
  .old_g(stageMEM.ctrl_in.old_g),
  .old_p(stageMEM.ctrl_in.old_p),
  .take(pred_hit),
  .take_lgp(take_lgp),
  .target_addr(bht_out)
);


always_comb begin : forwarding
    forwardA = forwardmux::forward_none;
    forwardB = forwardmux::forward_none;
    
    if(stageMEM.rd_out != '0 && stageMEM.ctrl_out.load_regfile)
    begin
        if(stageID.ctrl_out.rs1 == stageMEM.rd_out)
        begin
            forwardA = forwardmux::forward_mem;
        end
        if(stageID.ctrl_out.rs2 == stageMEM.rd_out)
        begin
            forwardB = forwardmux::forward_mem;
        end
    end

    if(stageEX.rd_out != '0 && stageEX.ctrl_out.load_regfile)
    begin
        if(stageID.ctrl_out.rs1 == stageEX.rd_out)
        begin
            forwardA = forwardmux::forward_ex;
        end
        if(stageID.ctrl_out.rs2 == stageEX.rd_out)
        begin
            forwardB = forwardmux::forward_ex;
        end
    end
end

always_comb begin
    unique case (forwardA) 
        forwardmux::forward_none :
            stageEX.rs1_out = stageID.rs1_out;
        forwardmux::forward_ex :
            stageEX.rs1_out = stageEX.forward_ex;
        forwardmux::forward_mem :
            stageEX.rs1_out = stageMEM.forward_mem;
        default :
            `BAD_MUX_SEL;
    endcase

    unique case (forwardB) 
        forwardmux::forward_none :
            stageEX.rs2 = stageID.rs2_out;
        forwardmux::forward_ex :
            stageEX.rs2 = stageEX.forward_ex;
        forwardmux::forward_mem :
            stageEX.rs2 = stageMEM.forward_mem;
        default :
            `BAD_MUX_SEL;
    endcase
end

always_comb begin : nop
    nop_en = 1'b0;
    if (!br_flush && stageID.ctrl_out.mem_read && (stageID.rd_reg_out == stageIF.rs1 || stageID.rd_reg_out == stageIF.rs2)) begin
        nop_en = 1'b1;
    end
end : nop

endmodule : datapath
