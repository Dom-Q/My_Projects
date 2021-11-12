#include "std_driver.h"

// STDD in driver
int32_t stdin_open(const uint8_t* filename){
    return -1;
}

int32_t stdin_close(int32_t fd){
    return -1;
}

int32_t stdin_write(int32_t fd, const void* buf, int32_t length){
    return -1;
}

// Read from buffer
int32_t stdin_read(int32_t fd, void* buf, int32_t length){
    return terminal_read(fd, buf, length);
}

// STD out driver
int32_t stdout_open(const uint8_t* filename){
    return -1;
}

int32_t stdout_close(int32_t fd){
    return -1;
}

// Fills buffer
int32_t stdout_write(int32_t fd, const void* buf, int32_t length){
    return terminal_write(fd, buf, length);
}

int32_t stdout_read(int32_t fd, void* buf, int32_t length){
    return -1;
}