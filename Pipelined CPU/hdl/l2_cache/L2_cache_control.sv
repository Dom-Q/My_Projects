/* MODIFY. The cache controller. It is a state machine
that controls the behavior of the cache. */

module L2_cache_control (
    input clk,
    input rst,
    input logic hit,
    input logic dirty,
    input logic mem_read,
    input logic mem_write,
    input logic pmem_resp,
    output logic dataoutmux_sel,
    output logic ld_lru,
    output logic write_data,
    output logic address_sel,
    output logic [1:0] wenablemux_sel,
    output logic mem_resp,
    output logic data_in_sel,
    output logic pmem_read,
    output logic pmem_write,
    output logic dirty_in,
    output logic ld_dirty,
    output logic way_sel
);

enum int unsigned {
    /* List of states */
    IDLE, COMPARE, ALLOCATE, WRITE_BACK, HOLD, HOLD2
} state, next_states;

function void set_defaults();
    way_sel = 1'b0;
    dataoutmux_sel = 1'b1;
    wenablemux_sel = 2'b00;
    mem_resp = 1'b0;
    pmem_read = 1'b0;
    pmem_write = 1'b0; 
    ld_lru = 1'b0; 
    address_sel = 1'b1;
    data_in_sel = 1'b0;  
    ld_dirty = 1'b0;
    dirty_in = 1'b0;
    write_data = 1'b0;
endfunction

always_comb
begin : state_actions
    /* Default output assignments */
    set_defaults();
    /* Actions for each state */
    unique case (state)
        IDLE: ;
        COMPARE: begin
            if (hit) begin
                if (mem_write) begin
                    wenablemux_sel = 2'b01;
                    way_sel = 1'b1;
                    dirty_in = 1'b1;
                    ld_dirty = 1'b1;
                end
                mem_resp = 1'b1;
                ld_lru = 1'b1;
            end
        end
        ALLOCATE: begin
            wenablemux_sel = 2'b10;        
            pmem_read = 1'b1;
            data_in_sel = 1'b1;
        end
        HOLD2: begin
            wenablemux_sel = 2'b10;
            data_in_sel = 1'b1;
            ld_dirty = 1'b1;
            write_data = 1'b1;
        end
        WRITE_BACK: begin            
            address_sel = 1'b0;
            dataoutmux_sel = 1'b0;
            pmem_write = 1'b1;            
        end
        HOLD: ;
        default: ;
    endcase
end


always_comb
begin : next_state_logic
    /* Next state information and conditions (if any)
     * for transitioning between states */
    //next_states = state;

    unique case (state)
        IDLE: begin
            if (mem_read ^ mem_write)
                next_states = COMPARE;
            else
                next_states = IDLE;
        end
        COMPARE: begin
            if (hit)
                next_states = IDLE;
            else if (dirty)
                next_states = WRITE_BACK;
            else
                next_states = ALLOCATE;
        end
        WRITE_BACK: begin
            if (pmem_resp)
                next_states = HOLD;
            else
                next_states = WRITE_BACK;
        end
        HOLD: begin
            if (pmem_resp)
                next_states = HOLD;
            else
                next_states = ALLOCATE;
        end
        ALLOCATE: begin
            if (pmem_resp)
                next_states = HOLD2;
            else
                next_states = ALLOCATE;
        end
        HOLD2: begin
            if (pmem_resp)
                next_states = HOLD2;
            else
                next_states = COMPARE;
        end
        default:
            next_states = IDLE;
    endcase
end

always_ff @(posedge clk)
begin: next_state_assignment
    /* Assignment of next state on clock edge */
    if (rst)
        state <= IDLE;
    else
        state <= next_states;
end

endmodule : L2_cache_control
