import rv32i_types::*;

module muldiv 
(
    input clk,
    input rst,
    input rv32i_word a, b,
    input logic muldiv_en,
    input logic mem_stall,
    input muldiv_funct3_t funct3,
    output rv32i_word f,
    output logic muldiv_resp
);

rv32i_word remainder, quotient, multiplicand, multiplier, divisor, dividend;
logic start_mul, start_div, div_done, mul_done, mul_ready, div_edge_case;
rv32i_double product_in, product;

enum int unsigned {
    IDLE,
    MUL,
    HOLD_MUL,
    DIV,
    DONE
} state, next_state;

add_shift_multiplier multiplier_module (
    .clk_i(clk),
    .reset_n_i(~rst),
    .multiplicand_i(multiplicand),
    .multiplier_i(multiplier),
    .start_i((start_mul && mul_ready)),
    .ready_o(mul_ready),
    .product_o(product_in),
    .done_o(mul_done)
);

register #(.width(64)) product_reg(
    .clk(clk),
    .rst(rst),
    .load(mul_done),
    .in(product_in),
    .out(product)
);

divider divider 
(
    .rst(rst),
    .clk(clk),
    .divisor(divisor),
    .dividend(dividend),
    .start_i((start_div && !div_edge_case)),
    .quotient(quotient),
    .remainder(remainder),
    .done_o(div_done)
);

always_comb begin : SET_INPUTS
    multiplier = 32'bx;
    multiplicand = 32'bx;
    divisor = 32'bx;
    dividend = 32'bx;
    div_edge_case = 1'b0;
    f = 32'bx;
    
    unique case(funct3)
        mul: begin          

            if (b[31] == 1)
                multiplier = -b;
            else
                multiplier = b;
            if (a[31] == 1)
                multiplicand = -a;
            else
                multiplicand = a;
            
            if (a[31] != b[31])
                f = -product[31:0];
            else
                f = product[31:0];
            
        end
        mulh: begin
            if (b[31] == 1)
                multiplier = -b;
            else
                multiplier = b;
            if (a[31] == 1)
                multiplicand = -a;
            else
                multiplicand = a;
            
            if (a[31] != b[31])
                f = -product[63:32];
            else
                f = product[63:32];
        end
        mulhsu: begin
            multiplier = b;

            if (a[31] == 1)
                multiplicand = -a;
            else
                multiplicand = a;

            if (a[31] == 1'b1)
                f = -product[63:32];
            else
                f = product[63:32];         
        end
        mulhu: begin
            multiplier = b;
            multiplicand = a;
            f = product[63:32];
        end
        div: begin
            if (b[31] == 1)
                divisor = -b;
            else
                divisor = b;
            if (a[31] == 1)
                dividend = -a;
            else
                dividend = a;

            if (b == 32'b0) begin
                f = -(32'b1);
                div_edge_case = 1'b1;
            end
            else if (b == -(32'b1) && a == -(32'd2**31)) begin
                f = a;
                div_edge_case = 1'b1;
            end
            else begin
                if (a[31] != b[31])
                    f = -quotient;
                else
                    f = quotient;
            end
        end  
        divu: begin
            divisor = b;
            dividend = a;
            if (b == 32'b0) begin
                f = 32'h2**32 - 1;
                div_edge_case = 1'b1;
            end
            else begin
                f = quotient;
            end
        end
        rem: begin
            if (b[31] == 1)
                divisor = -b;
            else
                divisor = b;
            if (a[31] == 1)
                dividend = -a;
            else
                dividend = a;

            if (b == 32'b0) begin
                f = a;
                div_edge_case = 1'b1;
            end
            else if (b == -(32'b1) && a == -(32'd2**31)) begin
                f = 0;
                div_edge_case = 1'b1;
            end
            else begin
                if (a[31] == 1'b1)
                    f = -remainder;
                else
                    f = remainder;
            end
        end   
        remu: begin
            divisor = b;
            dividend = a;
            if (b == 32'b0) begin
                f = a;
                div_edge_case = 1'b1;
            end else begin
                f = remainder;
            end
        end
        default: ; 
    endcase
end

always_ff @ (posedge clk)
begin
    if(rst)
        state <= IDLE;
    else
        state <= next_state;
end

always_comb begin : NEXT_STATE_LOGIC
    next_state = state;
    unique case(state)
        IDLE: begin
            if (muldiv_en) begin
                unique case(funct3)
                    mul, mulh, mulhu, mulhsu: begin
                        next_state = MUL;
                    end
                    div, divu, rem, remu: begin
                        next_state = DIV;
                    end
                    default: ;
                endcase
            end
        end
        MUL: begin
            if (mul_done) //TODO (add mul_edge_case)
                next_state = HOLD_MUL;
        end
        HOLD_MUL: begin
            if (mul_done) //TODO (add mul_edge_case)
                next_state = DONE;
        end
        DIV: begin
            if(div_done || div_edge_case)
                next_state = DONE;
        end
        DONE:
            if(!mem_stall)
                next_state = IDLE;
        default: ;
    endcase
end

always_comb begin : CONTROL_LOGIC
    start_mul = 1'b0;
    start_div = 1'b0;
    muldiv_resp = 1'b0;
    unique case(state)
        IDLE: ;
        MUL:
            start_mul = 1'b1;
        DIV:
            start_div = 1'b1;
        DONE:
            muldiv_resp = 1'b1;
        default: ;
    endcase
end

endmodule : muldiv