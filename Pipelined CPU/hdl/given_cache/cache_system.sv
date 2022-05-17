module cache_system(
    input clk,
    input rst,

    input logic [31:0] i_mem_addr,
    input logic i_mem_read,
    output logic [31:0] i_mem_rdata,
    output logic i_mem_resp,
    
    input logic d_mem_read,
    input logic d_mem_write,
    input logic [31:0] d_mem_addr,
    input logic [31:0] d_mem_wdata,
    input logic [3:0] d_mem_byte_enable,
    output logic [31:0] d_mem_rdata,
    output logic d_mem_resp,

    input logic mem_resp,
    input logic [63:0] mem_rdata,
    output logic [63:0] mem_wdata,
    output logic mem_read,
    output logic mem_write,
    output logic [31:0] mem_address
);

arbiter_ctrl::arbiter_itf arbiter_sig;

cache i_cache(
    .clk(clk),

    .pmem_resp(arbiter_sig.i_mem_resp),
    .pmem_rdata(arbiter_sig.i_mem_rdata),
    .pmem_address(arbiter_sig.i_mem_addr),
    .pmem_wdata(),
    .pmem_read(arbiter_sig.i_mem_read),
    .pmem_write(),

    .mem_read(i_mem_read),
    .mem_write(1'b0),
    .mem_byte_enable_cpu(4'bxxxx),
    .mem_address(i_mem_addr),
    .mem_wdata_cpu(32'bx),
    .mem_resp(i_mem_resp),
    .mem_rdata_cpu(i_mem_rdata)
);

cache d_cache(
    .clk(clk),

    .pmem_resp(arbiter_sig.d_mem_resp),
    .pmem_rdata(arbiter_sig.d_mem_rdata),
    .pmem_address(arbiter_sig.d_mem_addr),
    .pmem_wdata(arbiter_sig.d_mem_wdata),
    .pmem_read(arbiter_sig.d_mem_read),
    .pmem_write(arbiter_sig.d_mem_write),

    .mem_read(d_mem_read),
    .mem_write(d_mem_write),
    .mem_byte_enable_cpu(d_mem_byte_enable),
    .mem_address(d_mem_addr),
    .mem_wdata_cpu(d_mem_wdata),
    .mem_resp(d_mem_resp),
    .mem_rdata_cpu(d_mem_rdata)
);

arbiter cache_arbiter(
    .clk(clk),
    .rst(rst),
    
    .i_mem_addr(arbiter_sig.i_mem_addr),
    .i_mem_read(arbiter_sig.i_mem_read),
    .i_mem_rdata(arbiter_sig.i_mem_rdata),
    .i_mem_resp(arbiter_sig.i_mem_resp),
    
    .d_mem_read(arbiter_sig.d_mem_read),
    .d_mem_write(arbiter_sig.d_mem_write),
    .d_mem_addr(arbiter_sig.d_mem_addr),
    .d_mem_wdata(arbiter_sig.d_mem_wdata),
    .d_mem_rdata(arbiter_sig.d_mem_rdata),
    .d_mem_resp(arbiter_sig.d_mem_resp),

    .mem_resp(arbiter_sig.l2_mem_resp),
    .mem_rdata(arbiter_sig.l2_mem_rdata),
    .mem_address(arbiter_sig.l2_mem_addr),
    .mem_wdata(arbiter_sig.l2_mem_wdata),
    .mem_read(arbiter_sig.l2_mem_read),
    .mem_write(arbiter_sig.l2_mem_write)
);

logic adaptor_read;
logic adaptor_write;
logic adaptor_resp;
logic [31:0] adaptor_addr;
logic [255:0] adaptor_wdata;
logic [255:0] adaptor_rdata;

logic buf_read;
logic buf_write;
logic buf_resp;
logic [31:0] buf_addr;
logic [255:0] buf_wdata;
logic [255:0] buf_rdata;

L2_cache #(.s_index(5), .s_offset(5)) L2 (
    .clk(clk),
    .rst(rst),

    .mem_resp(arbiter_sig.l2_mem_resp),
    .mem_rdata(arbiter_sig.l2_mem_rdata),
    .mem_address(arbiter_sig.l2_mem_addr),
    .mem_wdata(arbiter_sig.l2_mem_wdata),
    .mem_read(arbiter_sig.l2_mem_read),
    .mem_write(arbiter_sig.l2_mem_write),

    .pmem_resp(buf_resp),
    .pmem_rdata(buf_rdata),
    .pmem_address(buf_addr),
    .pmem_wdata(buf_wdata),
    .pmem_read(buf_read),
    .pmem_write(buf_write)

    // .pmem_rdata(adaptor_rdata),
    // .pmem_wdata(adaptor_wdata),
    // .pmem_address(adaptor_addr),
    // .pmem_read(adaptor_read),
    // .pmem_write(adaptor_write),
    // .pmem_resp(adaptor_resp)
);

ewb eviction_write_buffer(
    .clk(clk),
    .rst(rst),

    .mem_resp(buf_resp),
    .mem_rdata(buf_rdata),
    .mem_address(buf_addr),
    .mem_wdata(buf_wdata),
    .mem_read(buf_read),
    .mem_write(buf_write),

    .pmem_rdata(adaptor_rdata),
    .pmem_wdata(adaptor_wdata),
    .pmem_address(adaptor_addr),
    .pmem_read(adaptor_read),
    .pmem_write(adaptor_write),
    .pmem_resp(adaptor_resp)
);

cacheline_adaptor adaptor(
    .clk(clk),
    .reset_n(~rst),

    // Port to LLC (Lowest Level Cache)
    .line_i(adaptor_wdata),
    .line_o(adaptor_rdata),
    .address_i(adaptor_addr),
    .read_i(adaptor_read),
    .write_i(adaptor_write),
    .resp_o(adaptor_resp),

    // Port to memory
    .burst_i(mem_rdata),
    .burst_o(mem_wdata),
    .address_o(mem_address),
    .read_o(mem_read),
    .write_o(mem_write),
    .resp_i(mem_resp)
);

endmodule : cache_system
