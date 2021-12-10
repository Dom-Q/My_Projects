#ifndef terminal
#define terminal

// void switch_terminal(int term_num);
void terminal_init();
void clear_char(int num_chars);
int32_t terminal_write(int32_t fd, const void *buf, int32_t length);
int32_t terminal_read(int32_t fd, void *buf, int32_t length);
int32_t terminal_open(const uint8_t *filename);
int32_t terminal_close(int32_t fd);
void initialize_keyboard();
void handle_input();
void clear_buffers();
void clear_cur_buffer();
void terminal_switch(int new_term);

#define MAX_CHARS 127
#define NUM_ROWS 24
#define PS2 0x60 //port to acccess all ps/2 devices
#define NUM_PRESSED_SCANCODES 59
#define SIZE_OF_TAB 3
#define NUM_COLUMNS 80
#define SIZE_OF_CUR_BUF 8
#define starter_len 7
#define MAX_HISTORY 100

typedef struct terminal_{
    char kbuf[MAX_CHARS];
    char typed_history[MAX_HISTORY][127];
    int num_in_history;
    char prev_typed_arr[127];
    int prev_typed;
    char terminal_buf[127];
    int enter_flag; 
    int num_up_pressed;
    int num_typed;
    int screen_x;
    int screen_y;
} terminal_;


extern terminal_ terminals[3];

#endif
