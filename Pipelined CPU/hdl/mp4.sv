module mp4(
    input clk,
    input rst,
    
    /* mem signals */
    input logic [63:0] mem_rdata,
    input logic mem_resp,
    output logic [63:0] mem_wdata,
    output logic [31:0] mem_address,
    output logic mem_read,
    output logic mem_write
);

/* i cache and d cache signals */
logic [31:0] i_mem_rdata;
logic [31:0] i_mem_addr;
logic i_mem_resp;
logic i_mem_read;
logic [31:0] d_mem_rdata;
logic [31:0] d_mem_wdata;
logic [31:0] d_mem_addr;
logic [3:0] d_mem_byte_enable;
logic d_mem_resp;
logic d_mem_read;
logic d_mem_write;

/* cache system */
cache_system cache(
    .clk(clk),
    .rst(rst),

    .i_mem_rdata(i_mem_rdata),
    .i_mem_resp(i_mem_resp),
    .i_mem_addr(i_mem_addr),
    .i_mem_read(i_mem_read),
    
    .d_mem_rdata(d_mem_rdata),
    .d_mem_resp(d_mem_resp),
    .d_mem_read(d_mem_read),
    .d_mem_write(d_mem_write),
    .d_mem_addr(d_mem_addr),
    .d_mem_wdata(d_mem_wdata),
    .d_mem_byte_enable(d_mem_byte_enable),

    .mem_resp(mem_resp),
    .mem_rdata(mem_rdata),
    .mem_wdata(mem_wdata),
    .mem_read(mem_read),
    .mem_write(mem_write),
    .mem_address(mem_address)
);
 
/* datapath */
datapath datapath
(
    .clk(clk),
    .rst(rst),

    .i_mem_rdata(i_mem_rdata),
    .i_mem_resp(i_mem_resp),
    .i_mem_addr(i_mem_addr),
    .i_mem_read(i_mem_read),
    
    .d_mem_rdata(d_mem_rdata),
    .d_mem_resp(d_mem_resp),
    .d_mem_read(d_mem_read),
    .d_mem_write(d_mem_write),
    .d_mem_addr(d_mem_addr),
    .d_mem_wdata(d_mem_wdata),
    .d_mem_byte_enable(d_mem_byte_enable)
);


endmodule : mp4
