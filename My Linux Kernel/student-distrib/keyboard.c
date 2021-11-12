#include "i8259.h"
#include "lib.h"
#include "terminal.h"

#define PS2 0x60 //port to acccess all ps/2 devices
#define MAX_CHARS 127
#define NUM_PRESSED_SCANCODES 59
#define SIZE_OF_TAB 3
#define NUM_ROWS 24
#define NUM_COLUMNS 80
#define SIZE_OF_CUR_BUF 8

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

static int caps_on, left_shift_pressed, right_shift_pressed, ctrl_pressed, num_typed, dont_print, starting_idx, alt_pressed;
// static char char_buffer[MAX_CHARS];
static char cur_buffer[1];

void clear_buffers(char *kbuf)
{
    int i;

    // initializes keyboard buffer
    for (i = 0; i < MAX_CHARS; i++)
        kbuf[i] = '\0';

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
    num_typed = 0;
    dont_print = 0;
    starting_idx = 0;
    alt_pressed = 0;
}

int handle_input(char *kbuf)
{
    asm volatile("pushal");
    cli();
    unsigned char scancode;
    scancode = inb(PS2);

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
        if (num_typed <= (MAX_CHARS - SIZE_OF_TAB))
            num_spaces = SIZE_OF_TAB;
        else
            num_spaces = MAX_CHARS - num_typed;

        for (i = 0; i < num_spaces; i++)
        {
            kbuf[num_typed] = space;
            cur_buffer[0] = space;
            terminal_write(0, cur_buffer, 1); // write 1 char to terminal
            clear_cur_buffer();
            num_typed++;
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
        if (get_screen_y() == NUM_ROWS)
        {
            scroll();
            move_bottom_left();
        }
        else
            printf("%c", '\n');
        num_typed = 0;
        clear_buffers(kbuf);
        update_cursor();
        return 1;
    }
    // backspace pressed
    else if (scancode == 14)
    {
        // ensures backspace is only done when not at 0,0
        // and when nothing else on line
        if (num_typed > 0)
        {
            // moves x,y back 1 index, then prints space, then moves x,y back
            int cur_x = get_screen_x();
            set_screen_x(cur_x - 1);
            printf("%c", ' ');
            set_screen_x(cur_x - 1);
            num_typed--;
            kbuf[num_typed] = '\0';
            update_cursor();
        }
    }
    // if screen-printable keys are pressed
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
                    terminal_write(0, kbuf, MAX_CHARS);
                    update_cursor();
                }
            }
            else
            {
                if (num_typed < MAX_CHARS)
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

                    kbuf[num_typed] = char_shown;
                    cur_buffer[0] = char_shown;
                    terminal_write(0, cur_buffer, 1); // write 1 char to terminal
                    clear_cur_buffer();
                    // printf("%c", char_shown);
                    num_typed++;
                }
            }
        }
    }

    send_eoi(1);
    sti();
    asm volatile("popal");
    return 0;
}
