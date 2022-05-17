module local_bp #
(
    parameter index = 3, // Number of hashes
    parameter N = 3 // Amount of history
)
(
    input clk,
    input rst,

    input logic update_history, // change branch status 
    input logic br_result, // 1 if actually taken, 0 if not

    input logic [index-1:0] rindex, //index this pc
    input logic [index-1:0] uindex, //update this pc

    //Outputs
    output logic take //take branch or not take branch?
);

int correct, wrong, total;

enum logic [1:0] {
    SNT = 2'b00,
    WNT = 2'b01,
    WT = 2'b10,
    ST = 2'b11
} state;

logic [1:0] r_state, u_state, new_state;

//local branch variables
logic [N-1:0] r_history; //used to index if PC history -> cntr
logic [N-1:0] u_history; //used to index mem PC history -> cntr

//history array
parallel_array #(.index(index), .width(N)) history(
    .clk(clk),
    .rst(rst),
    .load(update_history),
    .rindex(rindex),
    .uindex(uindex),
    .datain(((u_history << 1) | br_result)),
    .rdata_out(r_history),
    .udata_out(u_history)
);

//counter array
parallel_array #(.index(N), .width(2)) counter(
    .clk(clk),
    .rst(rst),
    .load(update_history),
    .rindex(r_history),
    .uindex(u_history),
    .datain(new_state),
    .rdata_out(r_state),
    .udata_out(u_state)
);

always_ff @(posedge clk) begin

    if (rst) begin
        total <= 0;
        correct <= 0;
        wrong <= 0;
    end

    if (update_history) begin
        total <= total + 1;
        if (br_result) begin
            unique case (u_state)
                SNT, WNT: wrong <= wrong + 1;
                WT, ST: correct <= correct + 1;
            default: ;
        endcase
        end else begin
            unique case (u_state)
                SNT, WNT: correct <= correct + 1;
                WT, ST: wrong <= wrong + 1;
            default:;
        endcase
        end
    end

end

always_comb begin : PREDICT_IF_PC
    unique case (r_state)
        SNT, WNT: take = 1'b0;
        WT, ST: take = 1'b1;
        default: take = 1'b0;
    endcase
end

always_comb begin : UPDATE_MEM_PC
    if (br_result) begin
        unique case (u_state)
            SNT: new_state = 2'b01;
            WNT: new_state = 2'b10;
            WT: new_state = 2'b11;
            ST: new_state = 2'b11;
        default: new_state = 2'b10;
    endcase
    end else begin
        unique case (u_state)
            SNT: new_state = 2'b00;
            WNT: new_state = 2'b00;
            WT: new_state = 2'b01;
            ST: new_state = 2'b10;
        default: new_state = 2'b01;
    endcase
    end
end

endmodule : local_bp

module global_bp #
(
    parameter index = 3, // Number of bits of pc
    parameter N = 3, // BHR length
    parameter L_index = index + N
)
(
    input clk,
    input rst,

    input logic update_history, // change branch status 
    input logic br_result, // 1 if actually taken, 0 if not

    input logic [index-1:0] rindex, //index this pc
    input logic [index-1:0] uindex, //update this pc

    //Outputs
    output logic take //take branch or not take branch?
);

logic [N-1:0] bhr;
logic [L_index-1:0] rl_index;
logic [L_index-1:0] ul_index;

assign rl_index = {bhr, rindex};
assign ul_index = {bhr, uindex};

//BHR
register #(.width(N)) bhr_reg (
    .clk(clk),
    .rst(rst),
    .load(update_history),
    .in(((bhr << 1) | br_result)),
    .out(bhr)
);

//local_bp
local_bp #(.index(L_index), .N(N)) local_history (
    .clk(clk),
    .rst(rst),

    .update_history(update_history),
    .br_result(br_result),
    .rindex(rl_index),
    .uindex(ul_index),
    .take(take)
);

endmodule : global_bp