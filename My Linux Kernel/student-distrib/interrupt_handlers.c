#include "interrupt_handlers.h"
#include "i8259.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"

// #define PS2 0x60 //port to acccess all ps/2 devices

// const unsigned char scancodes[60] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
//                                      'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
//                                      'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
//                                      'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' ', '\0'};

// const unsigned char caps_scancodes[60] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
//                                           'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
//                                           'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
//                                           'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'};

// const unsigned char shift_scancodes[60] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
//                                            'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
//                                            'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
//                                            'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'};

// const unsigned char caps_and_shift_scancodes[60] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
//                                                     'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0',
//                                                     'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '?', '~', '\0', '|',
//                                                     'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'};

// static int caps_on = 0;
// static int left_shift_pressed = 0;
// static int right_shift_pressed = 0;
// static int ctrl_pressed = 0;
// static int num_typed = 0;
// static int num_on_line = 0;
// static int char_buffer[128] = {'\0'};

void RTC_handler()
{
    asm volatile("pushal;");
    handle_rtc();
    asm volatile("popal;");
    asm("leave;"
        "iret;");
}

// void clear_buffer()
// {
//     int i;
//     for (i = 0; i < 128; i++)
//         char_buffer[i] = '\0';
// }
/*
    Function: keyboard_handler
    Description: function called when pic raised keyobard interupt
    Inputs: none
    Outpouts: none
    side_effects: takes input data from the ps/2 port and handles it
                for now prints scancode letter to screen
*/
void keyboard_handler()
{
    char kbuf[128] = {'\0'};
    handle_input(kbuf);

    // asm volatile("iret");
}
