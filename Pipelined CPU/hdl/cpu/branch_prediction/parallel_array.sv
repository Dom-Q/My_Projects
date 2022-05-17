/* DO NOT MODIFY. WILL BE OVERRIDDEN BY THE AUTOGRADER.
A register array to be used for tag arrays, LRU array, etc. */

module parallel_array #(
    parameter index = 3,
    parameter width = 1
)
(
    clk,
    rst,
    load,
    rindex,
    uindex,
    datain,
    rdata_out,
    udata_out
);

localparam num_sets = 2**index;

input clk;
input rst;
input load;
input [index-1:0] rindex;
input [index-1:0] uindex;
input [width-1:0] datain;
output logic [width-1:0] rdata_out;
output logic [width-1:0] udata_out;

logic [width-1:0] data [num_sets-1:0] /* synthesis ramstyle = "logic" */;

always_comb begin
    rdata_out = data[rindex];
    udata_out = data[uindex];
end

always_ff @(posedge clk)
begin
    if (rst) begin
        for (int i = 0; i < num_sets; ++i)
            data[i] <= '0;
    end
    else begin
        if(load)
            data[uindex] <= datain;
    end
end

endmodule : parallel_array