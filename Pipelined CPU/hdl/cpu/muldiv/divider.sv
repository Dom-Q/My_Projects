
import rv32i_types::*;

module divider 
(
    input clk,
    input rst,
    input rv32i_word divisor,
    input rv32i_word dividend,
    input logic start_i,
    output rv32i_word quotient,
    output rv32i_word remainder,
    output logic done_o
);

enum int unsigned {
    IDLE,
    START,
    SUB,
    END,
    DONE
} state, next_state;

logic ld_start, ld_end, ld_sub;
logic shift_in;
logic ld_div_reg;

int counter;

rv32i_word divrem;
rv32i_double divrem_out;
rv32i_word divisor_out;

rv32i_word sub_res;

assign remainder = divrem_out[63:32];
assign quotient = divrem_out[31:0];

shift_register shiftReg(
    .clk(clk),
    .rst(rst),
    .load_start(ld_start),
    .load_end(ld_end),
    .load_sub(ld_sub),
    .in(divrem),
    .shift_in(shift_in),
    .out(divrem_out)
);

register divisorReg (
    .clk(clk),
    .rst(rst),
    .load(ld_div_reg),
    .in(divisor),
    .out(divisor_out) 
);


always_comb begin
    ld_end = 0;
    ld_sub = 0;
    ld_div_reg = 0;
    ld_start = 0;

    done_o = 0;

    unique case(state)
        IDLE: ;
        START: begin
            ld_start = 1'b1;
            ld_div_reg = 1'b1;        
        end
        SUB: begin
            ld_sub = 1'b1;
                      
        end
        END: begin
            ld_end = 1'b1;            

        end
        DONE: begin
            done_o = 1'b1;
        end
    endcase
end

always_comb begin : divrem_in
    sub_res = remainder - divisor_out;

    if (state == START) begin
        divrem = dividend;
        shift_in = 1'bx;
    end
    else if (sub_res[31] == 1'b0) begin
        divrem = sub_res;
        shift_in = 1'b1;
    end 
    else begin
        divrem = remainder;
        shift_in = 1'b0;
    end
end

always_comb begin : NEXT_STATE_LOGIC
    next_state = state;

    unique case(state)

        IDLE: begin
            if (start_i) 
                next_state = START;
        end
        START:
            next_state = SUB;
        SUB: 
        begin
            if (counter >= 31)
                next_state = END;
            else
                next_state = SUB;
        end
        END:
            next_state = DONE;
        DONE:
            next_state = IDLE;
    endcase
end

always_ff @ (posedge clk)
begin
    if(rst) begin
        counter <= 0;
        state <= IDLE;
    end
    else
        state <= next_state;

    if (state == SUB)
        counter <= counter + 1;

    if (state == START)
        counter <= 0;
end

endmodule : divider