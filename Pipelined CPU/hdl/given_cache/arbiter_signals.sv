package arbiter_ctrl;

typedef struct packed
{
    // i mem arbiter
    logic i_mem_read;
    logic i_mem_resp;
    logic [255:0] i_mem_rdata;
    logic [31:0] i_mem_addr;

    // d mem arbiter
    logic d_mem_read;
    logic d_mem_write;
    logic d_mem_resp;
    logic [31:0] d_mem_addr;
    logic [255:0] d_mem_rdata;
    logic [255:0] d_mem_wdata;

    //l2 cache signals
    logic l2_mem_read;
    logic l2_mem_write;
    logic l2_mem_resp;
    logic [31:0] l2_mem_addr;
    logic [255:0] l2_mem_rdata;
    logic [255:0] l2_mem_wdata;
} arbiter_itf;

// typedef struct packed {
//     logic i_mem_read;
//     logic [31:0] i_mem_addr;
//     logic d_mem_read;
//     logic d_mem_write;
//     logic [255:0] d_mem_wdata;
//     logic [31:0] d_mem_addr;
// } in_cache_sig;

// typedef struct packed {
//     logic i_mem_resp;
//     logic [255:0] i_mem_rdata;
//     logic d_mem_resp;
//     logic [255:0] d_mem_rdata;
// } out_cache_sig;

typedef struct packed {
    logic [255:0] mem_rdata;
    logic mem_resp;
} mem_out_sig_to_arb;

typedef struct packed {
    logic [255:0] mem_wdata;
    logic [31:0] mem_address;
    logic mem_read;
    logic mem_write;
} mem_in_sig_to_arb;

typedef struct packed {
    logic [255:0] mem_wdata;
    logic [31:0] mem_address;
    logic mem_read;
    logic mem_write;
    logic [255:0] mem_rdata;
    logic mem_resp;
} arb_reg_t;

endpackage

import arbiter_ctrl::*;

module arb_mem_reg
(
    input clk,
    input rst,
    input load,
    input arb_reg_t in,
    output arb_reg_t out
);

arb_reg_t data;

always_ff @(posedge clk)
begin
    if (rst)
    begin
        data <= '0;
    end
    else if (load)
    begin
        data <= in;
    end
    else
    begin
        data <= data;
    end
end

always_comb
begin
    out = data;
end

endmodule

module out_register
(
    input clk,
    input rst,
    input load,
    input mem_out_sig_to_arb in,
    output mem_out_sig_to_arb out
);

mem_out_sig_to_arb data;

always_ff @(posedge clk)
begin
    if (rst)
    begin
        data <= '0;
    end
    else if (load)
    begin
        data <= in;
    end
    else
    begin
        data <= data;
    end
end

always_comb
begin
    out = data;
end

endmodule