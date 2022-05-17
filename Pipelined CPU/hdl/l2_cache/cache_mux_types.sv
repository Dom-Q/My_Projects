package wenablemux;
typedef enum bit [1:0] {
    disable_write = 2'b00,
    w_enable = 2'b01,
    pmem_enable = 2'b10
} wenablemux_sel_t;
endpackage

package dataoutmux;
typedef enum bit {
    lru = 1'b0,
    hit = 1'b1
} dataoutmux_sel_t;
endpackage

