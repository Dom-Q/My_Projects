#include "lib.h"
#include "terminal.h"

// STDIN driver
int32_t stdin_open(const uint8_t* filename);
int32_t stdin_close(int32_t fd);
int32_t stdin_write(int32_t fd, const void* buf, int32_t length);
int32_t stdin_read(int32_t fd, void* buf, int32_t length);

// STDOUT driver
int32_t stdout_open(const uint8_t* filename);
int32_t stdout_close(int32_t fd);
int32_t stdout_write(int32_t fd, const void* buf, int32_t length);
int32_t stdout_read(int32_t fd, void* buf, int32_t length);
