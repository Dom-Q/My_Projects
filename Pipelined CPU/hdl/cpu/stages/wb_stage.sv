`define BAD_MUX_SEL $fatal("%0t %s %0d: Illegal mux select", $time, `__FILE__, `__LINE__)

import rv32i_types::*;

module wb_stage(
    input clk,
    input rst,
    input rv32i_word mem_rdata_in,
    input rv32i_control_word ctrl_in,
    input rv32i_word alu_out,
    input logic cmp_out,
    input rv32i_word u_imm,
    input rv32i_word pc_out,
    input logic stall_n,

    // synthesis translate_off
    /* rvfi input */
    input rvfi_mon_word rvfi_word,
    // synthesis translate_on

    output rv32i_word regfilemux_out

    
);

rv32i_word mdrreg_out;
logic [1:0] addr_idx;
logic [7:0] mdr_byte;
logic [15:0] mdr_half;

assign addr_idx = alu_out[1:0];

assign mdrreg_out = mem_rdata_in;
assign mdr_byte = mdrreg_out[8*addr_idx +: 8];

always_comb begin
    unique case (addr_idx)
        2'b00: mdr_half = mdrreg_out[15 : 0];
        2'b10: mdr_half = mdrreg_out[31 : 16];
        default : mdr_half = 16'b0;
    endcase

    unique case (ctrl_in.regfilemux_sel)
        regfilemux::alu_out: regfilemux_out = alu_out;
        regfilemux::br_en: regfilemux_out = cmp_out;
        regfilemux::u_imm: regfilemux_out = u_imm;
        regfilemux::lw: regfilemux_out = mdrreg_out;
        regfilemux::lb: regfilemux_out = {{24{mdr_byte[7]}}, mdr_byte};
        regfilemux::lbu: regfilemux_out = {24'b0, mdr_byte};
        regfilemux::lh: regfilemux_out = {{16{mdr_half[15]}}, mdr_half};
        regfilemux::lhu: regfilemux_out = {16'b0, mdr_half};
        regfilemux::pc_plus4: regfilemux_out = pc_out + 4;
        default: `BAD_MUX_SEL;
    endcase    
end

// synthesis translate_off
rvfi_mon_word rvfi_word_local;
always_comb
begin
    rvfi_word_local = rvfi_word;
    rvfi_word_local.rd_wdata = regfilemux_out;
    rvfi_word_local.commit = ((ctrl_in.opcode != '0) && stall_n) ? 1'b1 : 1'b0;
end
// synthesis translate_on

endmodule
