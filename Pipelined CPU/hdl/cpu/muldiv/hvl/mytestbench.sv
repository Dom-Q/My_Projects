// module mem(
//     input [31:0] pmem_address,
//     input logic [255:0] pmem_wdata,
//     output logic [255:0] pmem_rdata,
//     input logic pmem_read,
//     input logic pmem_write,
//     output logic pmem_resp
// );

//     logic [4:0] [255:0] hold;
//     logic [31:0] last_addr;
//     integer idx;

//     initial begin
//         hold[0] = {16{16'h600D}};
//         hold[1] = {16{16'hBEEF}};
//         hold[2] = {16{16'hABCD}};
//         hold[3] = {16{16'h1234}};
//         //idx = 0;
//     end

//     always_comb begin
//         case(pmem_address) 
//             32'h10000000: idx = 0;
//             32'h20000000: idx = 1;
//             32'h30000000: idx = 2;
//             32'h40000000: idx = 3;
//         endcase
//     end

//     always @(posedge pmem_read) begin
//         #10;
//         pmem_rdata = hold[pmem_address[31:28]-1];
//         pmem_resp = 1'b1;
//         #40;
//         pmem_resp = 1'b0;
//     end

//     always @(posedge pmem_write) begin
//         #10;
//         hold[pmem_address[31:28]-1] = pmem_wdata;
//         //last_addr = pmem_address;
//         pmem_resp = 1'b1;
//         #40;
//         pmem_resp = 1'b0;

//     end
        


// endmodule : mem

// module testbench();
//     reg rst;

// // Generate clk signal
// bit clk;
// always #5 clk = clk === 1'b0;

// /* CPU memory signals */
//     logic [31:0]    mem_address;
//     logic [31:0]    mem_rdata;
//     logic [31:0]    mem_wdata;
//     logic           mem_read;
//     logic           mem_write;
//     logic [3:0]     mem_byte_enable;
//     logic           mem_resp;

//     /* Physical memory signals */
//     logic [31:0]    pmem_address;
//     logic [255:0]   pmem_rdata;
//     logic [255:0]   pmem_wdata;
//     logic           pmem_read;
//     logic           pmem_write;
//     logic           pmem_resp;

// cache dut (
//     .clk (clk),
//     .rst (rst),
//     .mem_address (mem_address),
//     .mem_rdata(mem_rdata),
//     .mem_wdata(mem_wdata),
//     .mem_read(mem_read),
//     .mem_write(mem_write),
//     .mem_byte_enable(mem_byte_enable),
//     .mem_resp(mem_resp),

//     /* Physical memory signals */
//     .pmem_address(pmem_address),
//     .pmem_rdata(pmem_rdata),
//     .pmem_wdata(pmem_wdata),
//     .pmem_read(pmem_read),
//     .pmem_write(pmem_write),
//     .pmem_resp(pmem_resp)
    
// );

// mem mem_ph(.*);

// default clocking tb_clk @(negedge clk); endclocking

// task reset();
//     rst <= 1'b1;
//     repeat (5) @(tb_clk);
//     rst <= 1'b0;
//     repeat (5) @(tb_clk);
// endtask

// // DO NOT MODIFY CODE ABOVE THIS LINE



// logic [31:0] read_data;

// initial begin
//     $display("Starting Cache Tests");
//     mem_write <= 1'b0;
//     mem_read <= 1'b0;
    
//     reset();
    

//     //write miss - way 0
//     mem_address <= 32'h20000000;
//     mem_wdata <= 32'hDEADDEAD;
//     mem_byte_enable <= 4'b1100;
//     mem_write <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     @(tb_clk);
//     mem_write <= 1'b0;
//     @(tb_clk);

//     // verify the write
//     mem_address <= 32'h20000000;
//     mem_read <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     assert(mem_rdata == 32'hDEADBEEF) else begin
//     	$error("%0t TB: Read %0d, expected %0d", $time, mem_rdata, 32'hDEADBEEF);
//     end
//     @(tb_clk);
//     mem_read <= 1'b0;
//     @(tb_clk);

    
    
//     //write miss - way 1
//     mem_address <= 32'h10000000;
//     mem_wdata <= 32'hBADDBADD;
//     mem_byte_enable <= 4'b1100;
//     mem_write <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     @(tb_clk);
//     mem_write <= 1'b0;
//     @(tb_clk);

//     // verify the write
//     mem_address <= 32'h10000000;
//     mem_read <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     assert(mem_rdata == 32'hBADD600D) else begin
//     	$error("%0t TB: Read %0d, expected %0d", $time, mem_rdata, 32'hBADD600D);
// end
//     @(tb_clk);
//     mem_read <= 1'b0;
//     @(tb_clk);


//     //read miss with write back - way 0
//     mem_address <= 32'h30000000;
//     mem_read <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     assert(mem_rdata == 32'hABCDABCD) else begin

//     		$error("%0t TB: Read %0d, expected %0d", $time, mem_rdata, 32'hABCDABCD);
//     end
//     @(tb_clk);
//     mem_read <= 1'b0;
//     @(tb_clk);

//    //write miss with a write back - way 1
//     mem_address <= 32'h40000000;
//     mem_wdata <= 32'hDEADDEAD;
//     mem_byte_enable <= 4'b1100;
//     mem_write <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     @(tb_clk);
//     mem_write <= 1'b0;
//     @(tb_clk);

//     // verify the write
//     mem_address <= 32'h40000000;
//     mem_read <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     assert(mem_rdata == 32'hDEAD1234) else begin

//     		$error("%0t TB: Read %0d, expected %0d", $time, mem_rdata, 32'hDEAD1234);
//     end
//     @(tb_clk);
//     mem_read <= 1'b0;
//     @(tb_clk)

//     mem_address <= 32'h10000000;
//     mem_read <= 1'b1;
//     while(mem_resp == 1'b0) begin
//         @(tb_clk);
//     end
//     assert(mem_rdata == 32'hBADD600D) else begin

//     		$error("%0t TB: Read %0d, expected %0d", $time, mem_rdata, 32'hBADD600D);
//     end
//     @(tb_clk);
//     mem_read <= 1'b0;
//     @(tb_clk)

//     #100

//     $finish;
// end

// endmodule : testbench

import rv32i_types::*;

module testbench();
    reg rst;

// Generate clk signal
bit clk;
always #5 clk = clk === 1'b0;

/* Declaring variables */
logic [63:0] val;
logic [31:0] result;
logic [31:0] div_check;
logic start_i;
logic rem_sel;
logic done_o;
logic [31:0] divisor;
logic [31:0] dividend;

assign dividend = {28'b0, val[3:0]};
assign divisor = {28'b0, val[7:4]};
assign div_check = dividend / divisor;

divider dut (
    .clk            (clk),
    .rst          (rst),
    .divisor        (divisor),
    .dividend       (dividend),
    .start_i        (start_i),
    .rem_sel        (rem_sel),
    .result         (result),
    .done_o         (done_o)
);

default clocking tb_clk @(negedge clk); endclocking

task reset();
    rst <= 1'b1;
    repeat (5) @(tb_clk);
    rst <= 1'b0;
    repeat (5) @(tb_clk);
endtask

initial begin
    $display("Starting divider test");
    start_i = '0;
    rem_sel = '0;
    val = {4'b0001, 4'b0000};

    reset();

    /* check product block */
    /* loop for every combination of input */
    while(val <= 8'hFF) begin

        if (val[7:4] == 0)
            continue;
        
        /* set start to high */
        @(tb_clk);
        start_i <= 1'b1;
        
        while(done_o != 1'b1)
            @(tb_clk);

        start_i = 1'b0;

        /* assert product is correct */
        assert(div_check == result)
        else begin
            if (done_o == 1'b1)
                $error ("%0t Error: Read %0d, expected %0d", $time, result, div_check);
        end

        @(tb_clk);
        val <= val + 1'b1;
        @(tb_clk);
    end

    $finish();
end

endmodule : testbench