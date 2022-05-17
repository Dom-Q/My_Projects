/* MODIFY. Your cache design. It contains the cache
controller, cache datapath, and bus adapter. */

module L2_cache #(
    parameter s_offset = 5,
    parameter s_index  = 3,
    parameter s_tag    = 32 - s_offset - s_index,
    parameter s_mask   = 2**s_offset,
    parameter s_line   = 8*s_mask,
    parameter num_sets = 2**s_index
)
(
    input clk,
    input rst,

    /* CPU memory signals */
    input   logic [31:0]    mem_address,
    output  logic [255:0]    mem_rdata,
    input   logic [255:0]    mem_wdata,
    input   logic           mem_read,
    input   logic           mem_write,
    output  logic           mem_resp,

    /* Physical memory signals */
    output  logic [31:0]    pmem_address,
    input   logic [255:0]   pmem_rdata,
    output  logic [255:0]   pmem_wdata,
    output  logic           pmem_read,
    output  logic           pmem_write,
    input   logic           pmem_resp
);

logic hit;
logic dirty, ld_lru, address_sel, ld_dirty, dirty_in, write_data, data_in_sel;
logic dataoutmux_sel, way_sel;
logic [1:0] wenablemux_sel;
logic [s_mask-1:0] mem_byte_enable256;
logic [255:0] mem_wdata256, data_out;

assign pmem_wdata = data_out;
assign mem_rdata = data_out;
assign mem_wdata256 = mem_wdata;
assign mem_byte_enable256 = {32{1'b1}};


L2_cache_control control
(
    .clk(clk),
    .rst(rst),
    .hit(hit),
    .dirty(dirty),
    .mem_read(mem_read),
    .mem_write(mem_write),
    .pmem_resp(pmem_resp),
    .dataoutmux_sel(dataoutmux_sel),
    .wenablemux_sel(wenablemux_sel),
    .ld_lru(ld_lru),
    .write_data(write_data),
    .ld_dirty(ld_dirty),
    .way_sel(way_sel),
    .data_in_sel(data_in_sel),
    .dirty_in(dirty_in),
    .address_sel(address_sel),
    .mem_resp(mem_resp),
    .pmem_read(pmem_read),
    .pmem_write(pmem_write)
);

L2_cache_datapath #(.s_index(s_index), .s_offset(s_offset)) datapath
(
    .clk(clk),
    .rst(rst),
    .hit(hit),
    .dirty(dirty),
    .mem_byte_enable256(mem_byte_enable256),
    .mem_address(mem_address),
    .dataoutmux_sel(dataoutmux_sel),
    .wenablemux_sel(wenablemux_sel),
    .ld_lru(ld_lru),
    .way_sel(way_sel),
    .ld_dirty(ld_dirty),
    .data_in_sel(data_in_sel),
    .write_data(write_data),
    .dirty_in(dirty_in),
    .address_sel(address_sel),
    .mem_wdata256(mem_wdata256),
    .pmem_rdata(pmem_rdata),
    .pmem_address(pmem_address),
    .data_out(data_out)
);

// bus_adapter bus_adapter
// (
    
//     .mem_rdata256(data_out),
//     .mem_wdata(mem_wdata),
//     .mem_byte_enable(mem_byte_enable),
//     .address(mem_address),
//     .mem_byte_enable256(mem_byte_enable256),
//     .mem_rdata(mem_rdata),
//     .mem_wdata256(mem_wdata256)
// );

endmodule : L2_cache
