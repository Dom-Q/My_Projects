module ewb (
    
    input clk,
    input rst,

    /*  L2 signals */
    input   logic [31:0]    mem_address,
    output  logic [255:0]    mem_rdata,
    input   logic [255:0]    mem_wdata,
    input   logic           mem_read,
    input   logic           mem_write,
    output  logic           mem_resp,

    /* Param memory signals */
    output  logic [31:0]    pmem_address,
    input   logic [255:0]   pmem_rdata,
    output  logic [255:0]   pmem_wdata,
    output  logic           pmem_read,
    output  logic           pmem_write,
    input   logic           pmem_resp
);

logic hit, ld_reg;
logic [31:0] mar_out;
logic [255:0] mdr_out;

assign hit = (mem_address == mar_out);

register mem_address_reg (
    .clk(clk),
    .rst(rst),
    .load(ld_reg),
    .in(mem_address),
    .out(mar_out)
);

register #(.width(256)) mem_data_reg (
    .clk(clk),
    .rst(rst),
    .load(ld_reg),
    .in(mem_wdata),
    .out(mdr_out)
);

enum int unsigned {
    IDLE,
    READ,
    WRITE,
    WAIT,
    WAIT_2
} state, next_state;

always_comb begin : CONTROL
    pmem_address = mem_address;
    pmem_read = mem_read;
    pmem_write = mem_write;
    pmem_wdata = mem_wdata;
    mem_resp = pmem_resp;
    mem_rdata = pmem_rdata;

    ld_reg = 1'b0;
    unique case(state)
        IDLE : begin
            if (mem_write) begin
                ld_reg = 1'b1;
                mem_resp = 1'b1;
                pmem_read = 1'b0;
                pmem_write = 1'b0;
            end
        end
        WAIT: begin
            if (hit && mem_read) begin
                pmem_read = 1'b0;
                pmem_write = 1'b0;
                mem_rdata = mdr_out;
                mem_resp = 1'b1;
            end
            if (hit && mem_write) begin
                pmem_read = 1'b0;
                pmem_write = 1'b0;
                ld_reg = 1'b1;
                mem_resp = 1'b1;
            end
        end
        WAIT_2: begin
            if (hit && mem_read) begin
                pmem_read = 1'b0;
                pmem_write = 1'b0;
                mem_rdata = mdr_out;
                mem_resp = 1'b1;
            end
            if (hit && mem_write) begin
                pmem_read = 1'b0;
                pmem_write = 1'b0;
                ld_reg = 1'b1;
                mem_resp = 1'b1;
            end
        end
        WRITE: begin
            pmem_address = mar_out;
            pmem_wdata = mdr_out;
            pmem_write = 1'b1;
            mem_resp = 1'b0;
            pmem_read = 1'b0;
            if (hit && mem_read) begin
                mem_rdata = mdr_out;
                mem_resp = 1'b1;
            end
        end
        default: ;
    endcase
end

always_comb begin : NEXT_STATE_LOGIC
    next_state = state;
    unique case(state)
        IDLE: begin 
            if (mem_write) begin
                next_state = WAIT;
            end
        end
        WAIT: begin 
            if (mem_read != 1'b1 && mem_write != 1'b1)
                next_state = WAIT_2;
        end
        WAIT_2: begin 
            if (mem_read != 1'b1 && mem_write != 1'b1)
                next_state = WRITE;
        end
        WRITE: begin
            if(pmem_resp)
                next_state = IDLE;
        end
        default: ;
    endcase
end

always @(posedge clk) begin : NEXT_STATE
    if(rst)
        state <= IDLE;
    else begin
        state <= next_state;
    end
end

endmodule