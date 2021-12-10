#include "std_driver.h"

/* 
*  Function: stdin_open
*  Input: string filename
*  Output: None
*  Return value: -1
*  Side effects: None
*/
int32_t stdin_open(const uint8_t* filename){
    return -1;
}

/* 
*  Function: stdin_close
*  Input: int file descriptor
*  Output: None
*  Return value: -1
*  Side effects: None
*/
int32_t stdin_close(int32_t fd){
    return -1;
}

/* 
*  Function: stdin_write
*  Input: int32_t fd, const void* buf, int32_t length
*  Output: None
*  Return value: -1
*  Side effects: None
*/
int32_t stdin_write(int32_t fd, const void* buf, int32_t length){
    return -1;
}

/* 
*  Function: stdin_read
*  Input: int32_t fd, const void* buf, int32_t length
*  Output: None
*  Return value: Number of bytes read
*  Side effects: Read from buffer
*/
int32_t stdin_read(int32_t fd, void* buf, int32_t length){
    return terminal_read(fd, buf, length); // read from terminal
}

/* 
*  Function: stdout_open
*  Input: string filename
*  Output: None
*  Return value: -1
*  Side effects: None
*/
int32_t stdout_open(const uint8_t* filename){
    return -1;
}

/* 
*  Function: stdout_close
*  Input: int file descriptor
*  Output: None
*  Return value: -1
*  Side effects: None
*/
int32_t stdout_close(int32_t fd){
    return -1;
}

/* 
*  Function: stdout_write
*  Input: int32_t fd, const void* buf, int32_t length
*  Output: None
*  Return value: number of bytes read
*  Side effects: write data to buffer
*/
int32_t stdout_write(int32_t fd, const void* buf, int32_t length){
    return terminal_write(fd, buf, length); // read from terminal
}

/* 
*  Function: stdout_read
*  Input: int32_t fd, void* buf, int32_t length
*  Output: None
*  Return value: -1
*  Side effects: None
*/
int32_t stdout_read(int32_t fd, void* buf, int32_t length){
    return -1;
}

