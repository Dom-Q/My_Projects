#include "keyboard.h"
#include "lib.h"

#define MAX_CHARS 127
#define NUM_ROWS 24

static char k_buffer[128]; // max characters including newline char

/* 
    Function: fill_buffer
    Description: fills global buffer based on char_buffer
    inputs: char_buffer
    outputs: none
    side_effects: sets global_buffer
*/
// void fill_buffer(char *char_buffer)
// {
//     int i;
//     for (i = 0; i < MAX_CHARS; i++)
//         k_buffer[i] = char_buffer[i];
//     k_buffer[127] = '\n';
//     return
// }

/* 
    Function: initialize_buffer
    Description: creates a null buffer to initializes the global buffer by calling fill buffer
    inputs: none
    outputs: none
    side_effects: sets global_buffer to null
*/
void initialize_kbuffer()
{
    char null_buffer[MAX_CHARS] = {'\0'};
    int i;
    for (i = 0; i < MAX_CHARS; i++)
        k_buffer[i] = null_buffer[i];
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
    char *char_buf = (char *)buf;
    int i;

    if (length <= 0)
        return -1;

    for (i = 0; i < length; i++)
    {
        if (char_buf[i] != '\0')
            printf("%c", char_buf[i]);
    }

    update_cursor();

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
    char kbuf[127] = {'\0'};
    char *char_buf = (char *)buf;
    while (handle_input(kbuf) != 1)
    {
    }

    int i;
    for (i = 0; i < 127; i++)
    {
        if (kbuf[i] != '\0')
        {
            char_buf[i] = kbuf[i];
        }
        else
            break;
    }
    int32_t ret_val = i + 1;
    char_buf[ret_val] = '\n';

    // char *char_buf = (char *)buf;
    // int32_t ret_val;
    // if (strlen(char_buf) < MAX_CHARS)
    //     ret_val = strlen(char_buf);
    // else
    //     ret_val = MAX_CHARS;

    // char null_buffer[sizeof(char_buf)] = {'\0'};
    // int i;
    // for (i = 0; i < MAX_CHARS; i++)
    //     k_buffer[i] = null_buffer[i];

    // int i;
    // for (i = 0; i < ret_val; i++)
    // {
    //     k_buffer[i] = char_buf[i];
    // }
    // k_buffer[ret_val] = '\n';
    // buf = k_buffer;

    // char_buf[ret_val] = '\n';
    //buf = (void *)char_buf;

    return ret_val;
}

int32_t terminal_open(const uint8_t *filename)
{
    return 0;
}

int32_t terminal_close(int32_t fd)
{
    return 0;
}
