import rv32i_types::*;

module register #(parameter width = 32)
(
    input clk,
    input rst,
    input load,
    input [width-1:0] in,
    output logic [width-1:0] out
);

logic [width-1:0] data = 1'b0;

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

endmodule : register

module ctrl_register
(
    input clk,
    input rst,
    input load,
    input rv32i_control_word in,
    output rv32i_control_word out
);

rv32i_control_word data = 1'b0;

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

endmodule : ctrl_register

module rvfi_register
(
    input clk,
    input rst,
    input load,
    input rvfi_mon_word in,
    output rvfi_mon_word out
);

rvfi_mon_word data = 1'b0;

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

endmodule : rvfi_register

module shift_register
(
    input clk,
    input rst,
    input load_start,
    input load_end,
    input load_sub,
    input rv32i_word in,
    input shift_in,
    output rv32i_double out
);

rv32i_double data = 1'b0;

always_ff @(posedge clk)
begin
    if (rst)
    begin
        data <= '0;
    end
    else if(load_start)
    begin
        data <= {32'b0, in};
    end
    else if(load_end) 
    begin
        data <= {in, {data[30:0], shift_in}};
    end
    else if (load_sub)
    begin
        data <= {{in[30:0], data[31:0]}, shift_in};
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

endmodule : shift_register