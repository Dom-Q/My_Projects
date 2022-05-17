`define BAD_MUX_SEL $fatal("%0t %s %0d: Illegal aribter select", $time, `__FILE__, `__LINE__)

import arbiter_ctrl::*;

module arbiter (
    input clk,
    input rst,

    /* i mem signals */
    input logic [31:0] i_mem_addr,
    input logic i_mem_read,
    output logic [255:0] i_mem_rdata,
    output logic i_mem_resp,

    /* d mem signals */
    input logic d_mem_read,
    input logic d_mem_write,
    input logic [31:0] d_mem_addr,
    input logic [255:0] d_mem_wdata,
    output logic [255:0] d_mem_rdata,
    output logic d_mem_resp,

    /* mem signals */
    input logic [255:0] mem_rdata,
    output logic [255:0] mem_wdata,
    output logic [31:0] mem_address,
    output logic mem_read,
    output logic mem_write,
    input logic mem_resp
);

arb_reg_t arb_reg_in, arb_reg_out;
logic ld_reg;

assign arb_reg_in.mem_rdata = mem_rdata;
assign arb_reg_in.mem_resp = mem_resp;

//assign i_mem_rdata = arb_reg_out.mem_rdata;
//assign d_mem_rdata = arb_reg_out.mem_rdata;

assign mem_wdata = arb_reg_out.mem_wdata;
assign mem_address = arb_reg_out.mem_address;
assign mem_read = arb_reg_out.mem_read;
assign mem_write = arb_reg_out.mem_write;

//assign arb_reg_in.mem_wdata = d_mem_wdata;

arb_mem_reg mem_reg_io(
    .clk(clk),
    .rst(rst),
    .load(ld_reg),
    .in(arb_reg_in),
    .out(arb_reg_out)
);

logic i_req, d_req;

assign i_req = i_mem_read;
assign d_req = d_mem_read ^ d_mem_write;

enum int unsigned {
    IDLE,
    INST,
    DATA,
    HOLD_I,
    HOLD_D,
    DONE_I,
    DONE_D
} state, next_state;

always_ff @ (posedge clk)
begin
    if(rst)
        state <= IDLE;
    else
        state <= next_state;
end

always_comb
begin
    next_state = state;

    unique case(state)
        IDLE: begin
            if(i_req) //inst request
                next_state = INST;
            else if(d_req) //data request
                next_state = DATA;
        end
        HOLD_I: begin
            if(mem_resp)
                next_state = DONE_I;
        end
        HOLD_D: begin
            if(mem_resp)
                next_state = DONE_D;
        end
        INST: begin
            next_state = HOLD_I;
        end
        DATA: begin
            next_state = HOLD_D;
        end
        default: begin
            next_state = IDLE;
        end
    endcase
end

always_comb begin
    ld_reg = 1'b0;
    arb_reg_in.mem_read = 1'b0;
    arb_reg_in.mem_write = 1'b0;
    arb_reg_in.mem_address = d_mem_addr;
    i_mem_resp = 1'b0;
    d_mem_resp = 1'b0;

    i_mem_rdata = 256'bx;
    d_mem_rdata = 256'bx;
    arb_reg_in.mem_wdata = 256'bx;

    unique case(state)
        IDLE: ;
        INST: begin
            arb_reg_in.mem_read = 1'b1;
            arb_reg_in.mem_address = i_mem_addr;
            //i_mem_resp = arb_reg_out.mem_resp;
            ld_reg = 1'b1;
        end
        DATA: begin
            arb_reg_in.mem_read = d_mem_read;
            arb_reg_in.mem_write = d_mem_write;
            arb_reg_in.mem_wdata = d_mem_wdata;
            //d_mem_resp = arb_reg_out.mem_resp;
            ld_reg = 1'b1;
        end
        HOLD_I: begin
            arb_reg_in.mem_read = 1'b1;
            arb_reg_in.mem_address = i_mem_addr;
            ld_reg = 1'b1;
        end
        HOLD_D: begin
            arb_reg_in.mem_read = d_mem_read;
            arb_reg_in.mem_write = d_mem_write;
            arb_reg_in.mem_wdata = d_mem_wdata;
            ld_reg = 1'b1;
        end
        DONE_I: begin
            i_mem_resp = 1'b1;
            i_mem_rdata = arb_reg_out.mem_rdata;
        end
        DONE_D: begin
            d_mem_resp = 1'b1;
            d_mem_rdata = arb_reg_out.mem_rdata;
        end
        default: ;
    endcase
end

endmodule : arbiter