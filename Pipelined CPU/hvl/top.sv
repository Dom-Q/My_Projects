module mp4_tb;
`timescale 1ns/10ps

/********************* Do not touch for proper compilation *******************/
// Instantiate Interfaces
tb_itf itf();
rvfi_itf rvfi(itf.clk, itf.rst);

// Instantiate Testbench
source_tb tb(
    .magic_mem_itf(itf),
    .mem_itf(itf),
    .sm_itf(itf),
    .tb_itf(itf),
    .rvfi(rvfi)
);

// For local simulation, add signal for Modelsim to display by default
// Note that this signal does nothing and is not used for anything
bit f;

/****************************** End do not touch *****************************/

/************************ Signals necessary for monitor **********************/
// This section not required until CP2

assign rvfi.commit = dut.datapath.WB.rvfi_word_local.commit; // Set high when a valid instruction is modifying regfile or PC
assign rvfi.halt = dut.datapath.WB.rvfi_word_local.halt;   // Set high when you detect an infinite loop
initial rvfi.order = 0;
always @(posedge itf.clk iff rvfi.commit) rvfi.order <= rvfi.order + 1; // Modify for OoO
int total, wrong, correct;
initial total = 0;
initial correct = 0;
initial wrong = 0;

always @(posedge itf.clk iff (!dut.datapath.rst && dut.datapath.stall_n && (dut.datapath.MEM.ctrl_in.br_en || dut.datapath.MEM.ctrl_in.br_jal)))
begin
    if (dut.datapath.predictor.hit2) begin
        total = total + 1;
        if (dut.datapath.MEM.br_flush) begin
            wrong = wrong + 1;
        end else begin
            correct = correct + 1;
        end
    end
end

/*
The following signals need to be set:
Instruction and trap:
    rvfi.inst
    rvfi.trap

Regfile:
    rvfi.rs1_addr
    rvfi.rs2_add
    rvfi.rs1_rdata
    rvfi.rs2_rdata
    rvfi.load_regfile
    rvfi.rd_addr
    rvfi.rd_wdata

PC:
    rvfi.pc_rdata
    rvfi.pc_wdata

Memory:
    rvfi.mem_addr
    rvfi.mem_rmask
    rvfi.mem_wmask
    rvfi.mem_rdata
    rvfi.mem_wdata

Please refer to rvfi_itf.sv for more information.
*/


// assign rvfi.inst = dut.datapath.IF.IR.data;
// assign rvfi.trap = dut.datapath.ID.controlRom.trap;
// assign rvfi.rs1_addr = dut.datapath.IF.rs1;
// assign rvfi.rs2_addr = dut.datapath.IF.rs2;
// assign rvfi.rs1_rdata = rvfi.rs1_addr ? dut.datapath.ID.rs1_out : 0;
// assign rvfi.rs2_rdata = rvfi.rs1_addr ? dut.datapath.ID.rs1_out : 0;
// assign rvfi.load_regfile = 1'b1;
// assign rvfi.rd_addr = dut.datapath.ID.load_regfile ? dut.datapath.ID.rd : 5'h0;
// assign rvfi.rd_wdata = rvfi.rd_addr ? dut.datapath.ID.regfilemux_out : 0;
// assign rvfi.pc_rdata = dut.datapath.IF.pc2_out;
// assign rvfi.pc_wdata = dut.datapath.IF.pc_mux_out;
// assign rvfi.mem_addr = dut.i_mem_addr;
// assign rvfi.mem_rmask = '0;
// assign rvfi.mem_wmask = '0;
// assign rvfi.mem_rdata = dut.i_mem_rdata;
// assign rvfi.mem_wdata = dut.d_mem_wdata;

// to be added
assign rvfi.inst = dut.datapath.WB.rvfi_word_local.inst;
assign rvfi.trap = dut.datapath.WB.rvfi_word_local.trap;
assign rvfi.rs1_addr = dut.datapath.WB.rvfi_word_local.rs1_addr;
assign rvfi.rs2_addr = dut.datapath.WB.rvfi_word_local.rs2_addr;
assign rvfi.rs1_rdata = dut.datapath.WB.rvfi_word_local.rs1_addr ? dut.datapath.WB.rvfi_word_local.rs1_rdata : '0;
assign rvfi.rs2_rdata = dut.datapath.WB.rvfi_word_local.rs2_addr ? dut.datapath.WB.rvfi_word_local.rs2_rdata :'0;
assign rvfi.load_regfile = dut.datapath.WB.rvfi_word_local.load_regfile;
assign rvfi.rd_addr = dut.datapath.WB.rvfi_word_local.load_regfile ? dut.datapath.WB.rvfi_word_local.rd_addr : 5'h0;
assign rvfi.rd_wdata = dut.datapath.WB.rvfi_word_local.rd_addr ? dut.datapath.WB.rvfi_word_local.rd_wdata : '0;
assign rvfi.pc_rdata = dut.datapath.WB.rvfi_word_local.pc_rdata;
assign rvfi.pc_wdata = dut.datapath.WB.rvfi_word_local.pc_wdata;
assign rvfi.mem_addr = dut.datapath.WB.rvfi_word_local.mem_addr;
assign rvfi.mem_rmask = dut.datapath.WB.rvfi_word_local.mem_rmask;
assign rvfi.mem_wmask = dut.datapath.WB.rvfi_word_local.mem_wmask;
assign rvfi.mem_rdata = dut.datapath.WB.rvfi_word_local.mem_rdata;
assign rvfi.mem_wdata = dut.datapath.WB.rvfi_word_local.mem_wdata;

/**************************** End RVFIMON signals ****************************/

/********************* Assign Shadow Memory Signals Here *********************/
// This section not required until CP2
/*
The following signals need to be set:
icache signals:
    itf.inst_read
    itf.inst_addr
    itf.inst_resp
    itf.inst_rdata

dcache signals:
    itf.data_read
    itf.data_write
    itf.data_mbe
    itf.data_addr
    itf.data_wdata
    itf.data_resp
    itf.data_rdata

Please refer to tb_itf.sv for more information.
*/

assign itf.inst_read = dut.i_mem_read;
assign itf.inst_addr = dut.i_mem_addr;
assign itf.inst_resp = dut.i_mem_resp;
assign itf.inst_rdata = dut.i_mem_rdata;

// dcache signals
assign itf.data_read = dut.d_mem_read;
assign itf.data_write = dut.d_mem_write;
assign itf.data_mbe = dut.d_mem_byte_enable;
assign itf.data_addr = dut.d_mem_addr;
assign itf.data_wdata = dut.d_mem_wdata;
assign itf.data_resp = dut.d_mem_resp;
assign itf.data_rdata = dut.d_mem_rdata;

/*********************** End Shadow Memory Assignments ***********************/

// Set this to the proper value
assign itf.registers = dut.datapath.ID.regFile.data;

/*********************** Instantiate your design here ************************/
/*
The following signals need to be connected to your top level:
Clock and reset signals:
    itf.clk
    itf.rst

Burst Memory Ports:
    itf.mem_read
    itf.mem_write
    itf.mem_wdata
    itf.mem_rdata
    itf.mem_addr
    itf.mem_resp

Please refer to tb_itf.sv for more information.
*/

mp4 dut(
    .clk(itf.clk),
    .rst(itf.rst),
    .mem_resp        (itf.mem_resp),
    .mem_rdata       (itf.mem_rdata),
    .mem_read        (itf.mem_read),
    .mem_write       (itf.mem_write),
    .mem_address     (itf.mem_addr),
    .mem_wdata       (itf.mem_wdata)
);

/***************************** End Instantiation *****************************/
initial begin
    $dumpfile("comp1.vcd");
    $dumpvars;
end

endmodule
