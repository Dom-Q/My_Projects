
module branch_prediction #(
    parameter index = 3,

    parameter btb_index = 6,
    parameter N = 3
)
(
  input clk,
  input rst,

  input logic update_history, // change branch status 
  input logic br_result, // incrememnt if high, decrement low

  input logic [31:0] if_pc, // branch instruction address. Last 2 bits reserved for prediction state
  input logic [31:0] mem_pc,
  input logic [31:0] target_addr_in,
  input logic old_l,
  input logic old_g,
  input logic old_p,
  
  //Outputs
  output logic take,
  output logic [2:0] take_lgp,
  output logic [31:0] target_addr
);

int correct, wrong, total;

logic pred_mux_sel, take_l, take_g, hit, hit2;
logic [index-1:0] r_index, u_index;
logic [btb_index-1:0] btb_rindex, btb_uindex;
logic [31-btb_index:0] btb_tag_out, u_tagout;
logic right_l, right_g;
logic [31:0] target_addr_2;

assign hit2 = '0;//(u_tagout == mem_pc[31:btb_index] && target_addr_2 != '0);

assign r_index = if_pc[index-1:0];
assign u_index = mem_pc[index-1:0];
assign btb_rindex = if_pc[btb_index-1:0];
assign btb_uindex = mem_pc[btb_index-1:0];

assign hit = (btb_tag_out == if_pc[31:btb_index] && target_addr != '0); 
assign take = hit && (pred_mux_sel ? take_g : take_l); // 0 = local, 1 = global

assign right_l = (old_l == br_result);
assign right_g = (old_g == br_result);

assign take_lgp = {take_l, take_g, pred_mux_sel};

enum logic [1:0] {
    SL = 2'b00,
    WL = 2'b01,
    WG = 2'b10,
    SG = 2'b11
} state;


logic[1:0] r_state, u_state, new_state;

//local_bp
local_bp #(.index(index), .N(N)) local_history (
    .clk(clk),
    .rst(rst),

    .update_history(update_history),
    .br_result(br_result),
    .rindex(r_index),
    .uindex(u_index),
    .take(take_l)
);

//global
global_bp #(.index(index), .N(N)) global_history(
    .clk(clk),
    .rst(rst),
    .update_history(update_history),
    .br_result(br_result),
    .rindex(r_index),
    .uindex(u_index),
    .take(take_g)
);

//counter
parallel_array #(.index(index), .width(2)) counter(
    .clk(clk),
    .rst(rst),
    .load((right_l ^ right_g) && update_history),
    .rindex(r_index),
    .uindex(u_index),
    .datain(new_state),
    .rdata_out(r_state),
    .udata_out(u_state)
);

//btb
btb_tag_bram btb_tag(
    .clock(clk),
    .wren(update_history && br_result),
    .rdaddress(btb_rindex),
    .wraddress(btb_uindex),
    .data(mem_pc[31:btb_index]),
    .q(btb_tag_out)
);



btb_data_bram btb_array(
    .clock(clk),
    .wren(update_history && br_result),
    .rdaddress(btb_rindex),
    .wraddress(btb_uindex),
    .data(target_addr_in),
    .q(target_addr)
);

always_ff @(posedge clk) begin
    if (rst) begin
        total <= 0;
        correct <= 0;
        wrong <= 0;
    end
    else if (update_history) begin
        total <= total + 1;
        if (right_l || right_g) begin
            correct <= correct + 1;
        end
        else begin
            wrong <= wrong + 1;
        end
    end
end

//tournament state logic
always_comb begin : PREDICT_IF_PC
    unique case (r_state)
        WL, SL: pred_mux_sel = 1'b0;
        WG, SG: pred_mux_sel = 1'b1;
        default: pred_mux_sel = 1'b0;
    endcase

end

always_comb begin : UPDATE_MEM_PC
    new_state = u_state;
    if (right_l ^ right_g) begin
        if(right_l) begin
            unique case (u_state)
                SL: new_state = 2'b00;
                WL: new_state = 2'b00;
                WG: new_state = 2'b01;
                SG: new_state = 2'b10;
                default: new_state = 2'b01;
            endcase
        end else begin
            unique case (u_state)
                SL: new_state = 2'b01;
                WL: new_state = 2'b10;
                WG: new_state = 2'b11;
                SG: new_state = 2'b11;
                default: new_state = WG;
            endcase
        end
    end
end

endmodule : branch_prediction