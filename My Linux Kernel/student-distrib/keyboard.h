#ifndef keyboard
#define keyboard

void initialize_keyboard();
int handle_input(char *kbuf);
void clear_buffers(char *kbuf);
void clear_cur_buffer();

#endif
