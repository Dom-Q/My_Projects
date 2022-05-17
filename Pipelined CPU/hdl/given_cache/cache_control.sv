module cache_control (
  input clk,

  /* CPU memory data signals */
  input  logic mem_read,
	input  logic mem_write,
	output logic mem_resp,

  /* Physical memory data signals */
  input  logic pmem_resp,
	output logic pmem_read,
	output logic pmem_write,

  /* Control signals */
  output logic tag_load,
  output logic valid_load,
  output logic dirty_load,
  output logic dirty_in,
  input logic dirty_out,

  input logic hit,
  output logic [1:0] writing
);

logic _tag_load;
logic _valid_load;
logic _dirty_load;
logic _dirty_in;

/* State Enumeration */
enum int unsigned
{
  check_hit,
	read_mem
} state, next_state;

/* State Control Signals */
always_comb begin : state_actions

	/* Defaults */
  _tag_load = 1'b0;
  _valid_load = 1'b0;
  _dirty_load = 1'b0;
  _dirty_in = 1'b0;
  writing = 2'b11;

	mem_resp = 1'b0;
	pmem_write = 1'b0;
	pmem_read = 1'b0;

	case(state)
    check_hit: begin
      if (mem_read || mem_write) begin
        if (hit) begin
          mem_resp = 1'b1;
          if (mem_write) begin
            _dirty_load = 1'b1;
            _dirty_in = 1'b1;
            writing = 2'b01;
          end
        end else begin
          if (dirty_out)
            pmem_write = 1'b1;
        end
      end
    end

    read_mem: begin
      pmem_read = 1'b1;
      writing = 2'b00;
      if (pmem_resp) begin
        _tag_load = 1'b1;
        _valid_load = 1'b1;
      end
        _dirty_load = 1'b1;
        _dirty_in = 1'b0;
    end

	endcase
end

/* Next State Logic */
always_comb begin : next_state_logic

	/* Default state transition */
	next_state = state;

	case(state)
    check_hit: begin
      if ((mem_read || mem_write) && !hit) begin
        if (dirty_out) begin
          if (pmem_resp)
            next_state = read_mem;
        end else begin
          next_state = read_mem;
		  end
      end
    end

    read_mem: begin
      if (pmem_resp)
        next_state = check_hit;
    end

	endcase
end

/* Next State Assignment */
always_ff @(posedge clk) begin: next_state_assignment
	 state <= next_state;
end

always_ff @(posedge clk) begin
  tag_load <= _tag_load;
  valid_load <= _valid_load;
  dirty_load <= _dirty_load;
  dirty_in <= _dirty_in;
end

endmodule : cache_control
