#ifndef terminal
#define terminal

// void fill_buffer(char *char_buffer);
void initialize_kbuffer();
int32_t terminal_write(int32_t fd, const char *buf, int32_t length);
int32_t terminal_read(int32_t fd, void *buf, int32_t length);
int32_t terminal_open(const uint8_t *filename);
int32_t terminal_close(int32_t fd);

#endif
