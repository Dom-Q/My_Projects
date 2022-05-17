`define BAD_MUX_SEL $fatal("%0t %s %0d: Illegal aribter select", $time, `__FILE__, `__LINE__)

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

logic i_req, d_req, flag;
logic [31:0] prefetch_addr, temp_buf;


enum int unsigned {
    IDLE,
    INST,
    DATA,
    PREFETCH
} state, next_state;

register prefetch_addr_reg (
    .clk(clk),
    .rst(rst),
    .load((mem_read) && (state != PREFETCH)),   
    .in((mem_address+32)),
    .out(prefetch_addr)
);

register check_reg (
    .clk(clk),
    .rst(rst),
    .load(flag),   
    .in(prefetch_addr),
    .out(temp_buf)
);

assign i_req = i_mem_read;
assign d_req = d_mem_read ^ d_mem_write;

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
            else if(prefetch_addr != '0 && temp_buf != prefetch_addr)
                next_state = PREFETCH;
        end
        INST: begin
            if(mem_resp)
                next_state = IDLE;
        end
                
        PREFETCH: begin
            if(mem_resp)
                next_state = IDLE;
        end
        DATA: begin
            if(mem_resp)
                next_state = IDLE;
        end
        default: begin
            next_state = IDLE;
        end
    endcase
end

assign mem_wdata = d_mem_wdata;
assign i_mem_rdata = mem_rdata;
assign d_mem_rdata = mem_rdata;

always_comb begin
    flag = 1'b0;
    unique case(state)
        IDLE: begin
            mem_read = 1'b0;
            mem_write = 1'b0;
            mem_address = d_mem_addr;
            i_mem_resp = 1'b0;
            d_mem_resp = 1'b0;
        end
        INST: begin
            mem_read = i_mem_read;
            mem_write = 1'b0;
            mem_address = i_mem_addr;
            i_mem_resp = mem_resp;
            d_mem_resp = 1'b0;
        end
        PREFETCH: begin
            flag = 1'b1;
            mem_read = 1'b1;
            mem_write = 1'b0;
            mem_address = {prefetch_addr[31:6], 5'b0};
            i_mem_resp = 1'b0;
            d_mem_resp = 1'b0;
        end
        DATA: begin
            mem_read = d_mem_read;
            mem_write = d_mem_write;
            mem_address = d_mem_addr;
            i_mem_resp = 1'b0;
            d_mem_resp = mem_resp;
        end
        default: begin
            mem_read = 1'b0;
            mem_write = 1'b0;
            mem_address = d_mem_addr;
            i_mem_resp = 1'b0;
            d_mem_resp = 1'b0;
        end
    endcase
end

endmodule : arbiter