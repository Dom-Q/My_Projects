#include "i8259.h"
#include "lib.h"
#include "terminal.h"
#include "system_calls.h"
#include "scheduling.h"
#define EIGHT_MB 0x00800000
#define EIGHT_KB 0x00002000

// char kbuf[MAX_CHARS] = {'\0'};
//int enter_flag = 0; // global var to check if enter was pressed

// char typed_history[MAX_HISTORY][127];

// static int num_in_history = 0;
//static int num_up_pressed = 0;
// static char prev_typed_arr[127] = {'\0'};
// static int prev_typed = 0;
// static int curr_terminal = 0;
// const char null_buf[127] = {'\0'};
// static char terminal_bufs[3][127];

terminal_ terminals[3];

void terminal_switch(int new_term)
{
    cli();
    // Update to new terminal state
    // if (!process[new_term])
    // {
    //     uint8_t shell[] = "shell0";
    //     shell[5] = '0' + new_term;
    //     terminals[running_proc].screen_x = get_screen_x();
    //     terminals[running_proc].screen_y = get_screen_y();
    //     running_proc = new_term;
    //     displayed_terminal = new_term;
    //     set_screen_x(terminals[running_proc].screen_x);
    //     set_screen_y(terminals[running_proc].screen_y);
    //     // update_vidmem(running_proc + 1);
    //     // update_vid_mem(running_proc);

    //     // update_cursor();
    //     do_execute(shell);
    // }
    // else

    page_tab[(VID_MEM_ADDR + (KB_4 * (displayed_terminal + 1))) >> EXTRA_BITS] = ((VID_MEM_ADDR + (KB_4 * (displayed_terminal + 1))) | 0x7);
    flush_tlb();

    memcpy((void *)(VID_MEM_ADDR + KB_4 * (displayed_terminal + 1)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);

   

    // page_tab[(VID_MEM_ADDR + (KB_4 * (displayed_terminal + 1))) >> EXTRA_BITS] = (VID_MEM_ADDR + (KB_4 * (displayed_terminal + 1))  | 0x7);
    // flush_tlb();

    // memcpy((void *)(VID_MEM_ADDR + KB_4 * (displayed_terminal + 1)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);

    memcpy((void *)VID_MEM_ADDR, (void *)(VID_MEM_ADDR + KB_4 * (new_term + 1)), (uint32_t)KB_4);

    page_tab[(VID_MEM_ADDR + (KB_4 * (new_term + 1))) >> EXTRA_BITS] = (VID_MEM_ADDR | 0x7);
    flush_tlb();
    
    
    terminals[displayed_terminal].screen_x = get_screen_x();
    terminals[displayed_terminal].screen_y = get_screen_y();
    set_screen_x(terminals[new_term].screen_x);
    set_screen_y(terminals[new_term].screen_y);
    update_vidmem(new_term + 1);

    
    // Save current PCB and load new
        // global_procs[running_proc] = *global_pcb;
    
    // Update virtual mem
        // physical memory offset for paging

    

    // Refresh page dir
   
    // Assign correct video memory pointer

    

    update_cursor();

    displayed_terminal = new_term;
    // if(displayed_terminal == running_proc){
    //     // update_vidmem(running_proc + 1, 1);
    //     //update paging
    //     update_vid_mem(running_proc);
    // }
    // else{
    //     // update_vidmem(1 + running_proc, 0);
    //     update_vid_mem(running_proc);
    //     //update paging
    // }

    

    // Update global PCB to new running process
    // Wait until end to avoid synchronization issues
    sti();
    return;
}
/*
    Function: Switch_terminal
    Description: to be called in scheduling.c changes the terminal
    input: terminal num
    output: none
    side_effect: terminal structs are switched
*/
// void switch_terminal(int term_num){
//     curr_terminal = term_num;
// }

/*
    Function: Terminal_init
    Description: to be called in kernel.c initializes the values inside of all 3 terminal structs
    input: none
    output: none
    side_effect: terminal structs are initialized
*/
void terminal_init()
{
    int i, j, k;
    displayed_terminal = 0;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < MAX_CHARS; j++)
        {
            terminals[i].kbuf[j] = '\0';
            terminals[i].prev_typed_arr[j] = '\0';
        }
        for (k = 0; k < MAX_HISTORY; k++)
        {
            terminals[i].typed_history[k][0] = '\0';
        }

        terminals[i].num_in_history = 0;
        terminals[i].prev_typed = 0;
        terminals[i].enter_flag = 0;
        terminals[i].num_up_pressed = 0;
        terminals[i].num_typed = 0;
        terminals[i].screen_x = 7;
        terminals[i].screen_y = 1;
    }
}

// array of scancode for various statuses of caps and shift
const unsigned char scancodes[NUM_PRESSED_SCANCODES] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
                                                        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
                                                        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
                                                        'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' ', '\0'};

const unsigned char caps_scancodes[NUM_PRESSED_SCANCODES] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
                                                             'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
                                                             'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
                                                             'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'};

const unsigned char shift_scancodes[NUM_PRESSED_SCANCODES] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
                                                              'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
                                                              'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
                                                              'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'};

const unsigned char caps_and_shift_scancodes[NUM_PRESSED_SCANCODES] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
                                                                       'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0',
                                                                       'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '?', '~', '\0', '|',
                                                                       'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'};

static int caps_on, left_shift_pressed, right_shift_pressed, ctrl_pressed, dont_print, starting_idx, alt_pressed; //flags
static char cur_buffer[1];

/* 
    Function: clear_char
    Description: clears the keyboard buffer and whats on screen by "num_chars" amount
    inputs: num_chars
    outputs: none
    side_effects: removes num_chars from the screen and keyboard buffer
*/
void clear_char(int num_chars)
{
    // ensures backspace is only done when not at 0,0
    // and when nothing else on line
    if (terminals[displayed_terminal].num_typed > 0)
    {
        int i;
        int cur_x = get_screen_x();
        // moves x,y back "num_chars" index, then prints space, then moves x,y back
        set_screen_x(cur_x - num_chars);
        for (i = 0; i < num_chars; i++)
        {
            printf("%c", ' ');
            terminals[displayed_terminal].num_typed--;
            terminals[displayed_terminal].kbuf[terminals[displayed_terminal].num_typed] = '\0';
        }
        set_screen_x(cur_x - num_chars);
        update_cursor();
    }
}

/* 
    Function: terminal_write
    Description: writes to the terminal based on the char_buffer
    inputs: char_buffer, starting_idx
    outputs: count: number of bytes printed
    side_effects: prints current char buffer to the terminal
*/
int32_t terminal_write(int32_t fd, const void *buf, int32_t length)
{
    cli();
    char *char_buf = (char *)buf;
    int i;
    
    if (length <= 0)
        return -1;

    for (i = 0; i < length; i++)
    {
        if (char_buf[i] != '\0')
        {
            
            putc(char_buf[i]);
           
        }
    }
    //uncommented this might cause issue
    sti();
    return length;
}

/* 
    Function: terminal_read
    Description: fills global buffer based on char_buffer
    inputs: char_buffer
    outputs: none
    side_effects: sets global_buffer by calling fill_buffer
*/
int32_t terminal_read(int32_t fd, void *buf, int32_t length)
{
    int i;
    int32_t bytes_read = 1; //  starts at 1 because technically, will always read newline character
    char *char_buf = (char *)buf;

    int len = strlen(buf);
    if (len > length)
        return -1;

    // initializing buffers to null
    for (i = 0; i < length; i++)
    {
        char_buf[i] = '\0';
    }
    for (i = 0; i < MAX_CHARS; i++)
    {
        terminals[displayed_terminal].kbuf[i] = '\0';
    }
    //CURRWENT ERROR
    // while(terms[curr_visible].enter_flag)
    //
    while (terminals[displayed_terminal].enter_flag == 0 || running_shell != displayed_terminal)
    {
    }

    for (i = 0; i < MAX_CHARS; i++)
    {
        if (terminals[displayed_terminal].kbuf[i] != '\0') // add 1 to bytes read counter if byte read is not null
            bytes_read++;
        char_buf[i] = terminals[displayed_terminal].kbuf[i];
    }
    char_buf[bytes_read - 1] = '\n';
    clear_buffers();
    terminals[displayed_terminal].enter_flag = 0;
    return bytes_read;
}

// terminal open and close do not do anything, so always result in success
int32_t terminal_open(const uint8_t *filename)
{
    return 0;
}

int32_t terminal_close(int32_t fd)
{
    return 0;
}

// clears the global buffer as well as the buffer that is passed into terminal write
void clear_buffers()
{
    int i;

    // initializes keyboard buffer
    for (i = 0; i < MAX_CHARS; i++)
        terminals[displayed_terminal].kbuf[i] = '\0';

    // initializes cur buffer, which is what is given to terminal write
    cur_buffer[0] = '\0';
}

void clear_cur_buffer()
{
    cur_buffer[0] = '\0';
}

// enables the keyboard irq by calling the enable_irq function
// also sets all flags to 0, and buffer to null
void initialize_keyboard()
{
    enable_irq(1);
    caps_on = 0;
    left_shift_pressed = 0;
    right_shift_pressed = 0;
    ctrl_pressed = 0;
    terminals[displayed_terminal].num_typed = 0;
    dont_print = 0;
    starting_idx = 0;
    alt_pressed = 0;
    // int i;
    // THIS IS CHANGED
    // moving this to inside of terminal init
    // for (i = 0; i < MAX_HISTORY; i++)
    // {
    //     typed_history[i][0] = '\0';
    // }
}

//
/* 
    Function: handle_input
    Description: when keyboard interrupt, this function handles the scancode from the interrupt
    inputs: none
    outputs: none
    side_effects: sets flags, changes global vars required for terminal_read, calls terminal_write
*/
void handle_input()
{
    cli();
    unsigned char scancode;
    scancode = inb(PS2);
    send_eoi(1);
    update_vidmem(displayed_terminal + 1);
    terminals[running_shell].screen_x = get_screen_x();
    terminals[running_shell].screen_y = get_screen_y();
    set_screen_x(terminals[displayed_terminal].screen_x);
    set_screen_y(terminals[displayed_terminal].screen_y);

    // printf("%d", scancode);

    // left shift key pressed down
    if (scancode == 42)
        left_shift_pressed = 1;

    // left shift key released
    else if (scancode == 170)
        left_shift_pressed = 0;

    // right shift key pressed down
    else if (scancode == 54)
        right_shift_pressed = 1;

    // right shift key released
    else if (scancode == 182)
        right_shift_pressed = 0;

    // caps lock key pressed, handles toggling state of caps lock
    else if (scancode == 58)
    {
        if (caps_on == 1)
            caps_on = 0;
        else
            caps_on = 1;
    }
    // tab pressed
    else if (scancode == 15)
    {
        char space = ' ';
        int i, num_spaces;

        // if less than 3 characters can be added bc of buffer
        // only add up to 127 chars
        // otherwise add 3 spaces
        if (terminals[displayed_terminal].num_typed <= (MAX_CHARS - SIZE_OF_TAB))
            num_spaces = SIZE_OF_TAB;
        else
            num_spaces = MAX_CHARS - terminals[displayed_terminal].num_typed;

        for (i = 0; i < num_spaces; i++)
        {
            terminals[displayed_terminal].kbuf[terminals[displayed_terminal].num_typed] = space;
            cur_buffer[0] = space;
            terminal_write(0, cur_buffer, 1); // write 1 char to terminal
            clear_cur_buffer();
            terminals[displayed_terminal].num_typed++;
        }
    }

    // control pressed down
    else if (scancode == 29)
        ctrl_pressed = 1;

    // control released
    else if (scancode == 157)
        ctrl_pressed = 0;

    // alt pressed down
    else if (scancode == 56)
        alt_pressed = 1;

    // alt released
    else if (scancode == 184)
        alt_pressed = 0;

    // enter pressed
    else if (scancode == 28)
    {
        int i;
        if (terminals[displayed_terminal].kbuf[0] != '\0')
        {
            // save what was typed to the keyboard buffer for hist buffer
            for (i = 0; i < 127; i++)
            {
                terminals[displayed_terminal].typed_history[terminals[displayed_terminal].num_in_history][i] = terminals[displayed_terminal].kbuf[i];
            }
            terminals[displayed_terminal].num_in_history++;
        }
        terminals[displayed_terminal].prev_typed = 0;
        terminals[displayed_terminal].num_up_pressed = 0;
        putc('\n');
        terminals[displayed_terminal].num_typed = 0;
        update_cursor();
        terminals[displayed_terminal].enter_flag = 1;
    }
    // backspace pressed
    else if (scancode == 14)
    {
        clear_char(1);
    }
    // up arrow pressed
    else if (scancode == 72)
    {

        if (terminals[displayed_terminal].num_up_pressed < MAX_HISTORY && terminals[displayed_terminal].num_in_history > terminals[displayed_terminal].num_up_pressed)
        {
            if (terminals[displayed_terminal].num_up_pressed == 0)
            {
                strncpy(terminals[displayed_terminal].prev_typed_arr, terminals[displayed_terminal].kbuf, MAX_CHARS);
                terminals[displayed_terminal].prev_typed = 1;
            }

            char *hist_char = terminals[displayed_terminal].typed_history[terminals[displayed_terminal].num_in_history - (terminals[displayed_terminal].num_up_pressed + 1)];

            if (hist_char[0] != '\0')
            {
                clear_char(terminals[displayed_terminal].num_typed);
                terminals[displayed_terminal].num_up_pressed++;
                int i;
                for (i = 0; i < MAX_CHARS; i++)
                {
                    terminals[displayed_terminal].kbuf[i] = hist_char[i];
                    if (hist_char[i] != '\0')
                    {
                        putc(hist_char[i]);
                        terminals[displayed_terminal].num_typed++;
                    }
                }
            }
        }
    }
    // down arrow pressed
    else if (scancode == 80)
    {

        if (terminals[displayed_terminal].num_up_pressed > 0)
        {
            //
            clear_char(terminals[displayed_terminal].num_typed);
            terminals[displayed_terminal].num_up_pressed--;

            if (terminals[displayed_terminal].num_up_pressed > 0)
            {
                char *hist_char = terminals[displayed_terminal].typed_history[terminals[displayed_terminal].num_in_history - terminals[displayed_terminal].num_up_pressed];

                if (hist_char[0] != '\0')
                {
                    int i;
                    for (i = 0; i < MAX_CHARS; i++)
                    {
                        terminals[displayed_terminal].kbuf[i] = hist_char[i];
                        if (hist_char[i] != '\0')
                        {
                            putc(hist_char[i]);
                            terminals[displayed_terminal].num_typed++;
                        }
                    }
                }
            }
            else if (terminals[displayed_terminal].prev_typed == 1)
            {
                int i;
                for (i = 0; i < MAX_CHARS; i++)
                {
                    terminals[displayed_terminal].kbuf[i] = terminals[displayed_terminal].prev_typed_arr[i];
                    if (terminals[displayed_terminal].prev_typed_arr[i] != '\0')
                    {
                        putc(terminals[displayed_terminal].prev_typed_arr[i]);
                        terminals[displayed_terminal].num_typed++;
                    }
                }
                terminals[displayed_terminal].prev_typed = 0;
            }
        }
    }

    //what is the terminal buf

    else if (scancode == 59 || scancode == 60 || scancode == 61)
    { // F1
        if (alt_pressed == 1)
        {
            int to_terminal = scancode - 59;
            if (displayed_terminal != to_terminal)
            {
                // Terminal switching to
                /*
                // strncpy(terminals[displayed_terminal].terminal_buf, terminals[displayed_terminal].kbuf, 127); // verify
                //disable_irq(0);
                // Check if first time opening terminal
                // asm volatile("\n\
                //         movl %%esp, %0 \n\
                //         movl %%ebp, %1"
                //                     : "=r"(global_pcb->curr_esp), "=r"(global_pcb->curr_ebp));
                */
                terminal_switch(to_terminal);
                /*
                // asm volatile("\n\
                //     movl %0, %%esp \n\
                //     movl %1, %%ebp"
                // :
                // : "r"(global_pcb->curr_esp), "r"(global_pcb->curr_ebp));
                */
                //enable_irq(0);

                // terminal_write(0, terminals[displayed_terminal].kbuf, 127);
            }
        }
    }
    // if screen-printable keys are presseds
    else
    {
        if (scancode <= NUM_PRESSED_SCANCODES)
        {
            if (ctrl_pressed == 1)
            {
                // if l pressed while ctrl pressed
                if (scancode == 38)
                {
                    // clear screen
                    clear();

                    // put typed chars back on screen and update cursor

                    char starter[starter_len] = "391OS> ";
                    terminal_write(0, starter, starter_len);
                    terminal_write(0, terminals[displayed_terminal].kbuf, MAX_CHARS);
                    update_cursor();
                }
                else if (scancode == 46) // check if c is pressed
                {
                    clear_char(terminals[displayed_terminal].num_typed);
                    putc('\n');
                    send_eoi(1);
                    do_halt(0);
                }
                else if (scancode == 22)
                {
                    clear_char(terminals[displayed_terminal].num_typed);
                }
                else if (scancode == 11)
                {
                    if (colourful_flag == 1)
                    {
                        colourful_flag = 0;
                    }
                    else
                    {
                        colourful_flag = 1;
                    }
                    // clear screen
                    clear();

                    // put typed chars back on screen and update cursor
                    char starter[starter_len] = "391OS> ";
                    terminal_write(0, starter, starter_len);
                    terminal_write(0, terminals[displayed_terminal].kbuf, MAX_CHARS);
                    update_cursor();
                }
                else if (scancode == 10)
                {
                    //int bufff[1] = {1024};
                    //rtc_open(0);
                    //rtc_write(0, bufff, 0);
                    launch_tests();
                }
            }
            else
            {
                if (terminals[displayed_terminal].num_typed < MAX_CHARS)
                {

                    char char_shown;
                    // if either shift is pressed along with caps lock toggled on
                    if (caps_on == 1 && (left_shift_pressed == 1 || right_shift_pressed == 1))
                        char_shown = caps_and_shift_scancodes[scancode];
                    // if caps lock toggled on
                    else if (caps_on == 1)
                        char_shown = caps_scancodes[scancode];
                    // if either shift pressed
                    else if (left_shift_pressed == 1 || right_shift_pressed == 1)
                        char_shown = shift_scancodes[scancode];
                    // if nothing is pressed besides a character
                    else
                        char_shown = scancodes[scancode];

                    terminals[displayed_terminal].kbuf[terminals[displayed_terminal].num_typed] = char_shown;
                    cur_buffer[0] = char_shown;
                    terminal_write(0, cur_buffer, 1); // write 1 char to terminal
                    clear_cur_buffer();
                    terminals[displayed_terminal].num_typed++;
                }
            }
        }
    }
    update_vidmem(running_shell + 1);
    terminals[displayed_terminal].screen_x = get_screen_x();
    terminals[displayed_terminal].screen_y = get_screen_y();
    set_screen_x(terminals[running_shell].screen_x);
    set_screen_y(terminals[running_shell].screen_y);
    sti();
}
