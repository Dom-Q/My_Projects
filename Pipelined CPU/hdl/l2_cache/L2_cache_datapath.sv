/* MODIFY. The cache datapath. It contains the data,
valid, dirty, tag, and LRU arrays, comparators, muxes,
logic gates and other supporting logic. */


module L2_cache_datapath #(
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
    input [31:0] mem_byte_enable256,
    input logic [31:0] mem_address,
    input logic dataoutmux_sel,
    input logic ld_lru,
    input logic dirty_in,
    input logic ld_dirty,
    input logic data_in_sel,
    input logic write_data,
    input logic address_sel,
    input logic way_sel,
    input logic [1:0] wenablemux_sel,
    input logic [255:0] mem_wdata256,
    input logic [255:0] pmem_rdata,
    output logic [31:0] pmem_address,
    output logic [255:0] data_out,
    output logic hit,
    output logic dirty
);

logic [255:0] datain;
logic [s_tag-1: 0] tag, ta0_out, ta1_out, dirty_tag;
logic [s_index-1: 0] index;
logic way0_sel, way1_sel;
logic lru_out, va0_out, va1_out, dirty0_out, dirty1_out;
logic [31:0] wenablemux_out, wenable_da0, wenable_da1, wenable_da0_lru, wenable_da1_lru, wenable_da0_hit, wenable_da1_hit;
logic hit_0, hit_1;
logic [255:0] da0_out, da1_out;

assign tag = mem_address[31:(s_offset+s_index)];
assign index = mem_address[(s_offset+s_index-1):s_offset];

assign hit_0 = ((tag == ta0_out) && va0_out);
assign hit_1 = ((tag == ta1_out) && va1_out);
assign hit = hit_1 ^ hit_0;

assign way0_sel = way_sel ? hit_0 : !lru_out;
assign way1_sel = way_sel ? hit_1 : lru_out;

assign dirty = lru_out ? dirty1_out : dirty0_out;
assign dirty_tag = lru_out ? ta1_out : ta0_out;

assign wenable_da0_lru = lru_out ? 32'b0 : wenablemux_out;
assign wenable_da1_lru = lru_out ? wenablemux_out : 32'b0;
assign wenable_da0_hit = hit_0 ? wenablemux_out : 32'b0;
assign wenable_da1_hit = hit_1 ? wenablemux_out : 32'b0;

assign wenable_da0 = way_sel ? wenable_da0_hit : wenable_da0_lru;
assign wenable_da1 = way_sel ? wenable_da1_hit : wenable_da1_lru;

assign datain = data_in_sel ? pmem_rdata : mem_wdata256;
assign pmem_address = address_sel ? {mem_address[31:s_offset], 5'b0} : {{dirty_tag, index}, 5'b0};


// Data Arrays
L2_data_array #(.s_offset(s_offset), .s_index(s_index)) DA0 
(
    .clk(clk),
    .read(1'b1),
    .write_en(wenable_da0),
    .rindex(index),
    .windex(index),
    .datain(datain),
    .dataout(da0_out)
);

L2_data_array #(.s_offset(s_offset), .s_index(s_index)) DA1 
(
    .clk(clk),
    .read(1'b1),
    .write_en(wenable_da1),
    .rindex(index),
    .windex(index),
    .datain(datain),
    .dataout(da1_out)
);

// Tag Arrays
L2_array #(.width(s_tag), .s_index(s_index)) TA0 
(
    .clk(clk),
    .rst(rst),
    .read(1'b1),
    .load((write_data && way0_sel)),
    .rindex(index),
    .windex(index),
    .datain(tag),
    .dataout(ta0_out)
);

L2_array #(.width(s_tag), .s_index(s_index)) TA1 
(
    .clk(clk),
    .rst(rst),
    .read(1'b1),
    .load((write_data && way1_sel)),
    .rindex(index),
    .windex(index),
    .datain(tag),
    .dataout(ta1_out)
);

// Valid Arrays
L2_array #(.s_index(s_index), .width(1)) VA0 
(
    .clk(clk),
    .rst(rst),
    .read(1'b1),
    .load((write_data && way0_sel)),
    .rindex(index),
    .windex(index),
    .datain(1'b1),
    .dataout(va0_out)
);

L2_array #(.s_index(s_index), .width(1)) VA1 
(
    .clk(clk),
    .rst(rst),
    .read(1'b1),
    .load((write_data && way1_sel)),
    .rindex(index),
    .windex(index),
    .datain(1'b1),
    .dataout(va1_out)
);

// Dirty Arrays
L2_array #(.s_index(s_index), .width(1)) DIA0 
(
    .clk(clk),
    .rst(rst),
    .read(1'b1),
    .load((ld_dirty && way0_sel)),
    .rindex(index),
    .windex(index),
    .datain(dirty_in),
    .dataout(dirty0_out)
);

L2_array #(.s_index(s_index), .width(1)) DIA1 
(
    .clk(clk),
    .rst(rst),
    .read(1'b1),
    .load((ld_dirty && way1_sel)),
    .rindex(index),
    .windex(index),
    .datain(dirty_in),
    .dataout(dirty1_out)
);

// LRU Arrays
L2_array #(.s_index(s_index), .width(1)) LRU
(
    .clk(clk),
    .rst(rst),
    .read(1'b1),
    .load(ld_lru),
    .rindex(index),
    .windex(index),
    .datain(hit_0),
    .dataout(lru_out)
);

//Muxes
always_comb begin : MUXES

    unique case (wenablemux_sel)
        2'b00 : wenablemux_out = 32'b0;
        2'b01 : wenablemux_out = mem_byte_enable256;
        2'b10 : wenablemux_out = {32{1'b1}};
        default : wenablemux_out = 32'b0;       
    endcase

    unique case (dataoutmux_sel)
        1'b0 : data_out = lru_out ? da1_out : da0_out;
        1'b1 : data_out = hit_1 ? da1_out : da0_out;
        default : data_out = hit_1 ? da1_out : da0_out;
    endcase

end

endmodule : L2_cache_datapath