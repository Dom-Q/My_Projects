#ifndef _FILE_SYTEM_DRIVER_H
#define _FILE_SYSTEM_DRIVER_H

#include "lib.h"
#include "file_system.h"
#include "system_calls.h"

// File driver functions
int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_write(int32_t fd, const void* buf, int32_t length);
int32_t file_read(int32_t fd, void* buf, int32_t length);

// Directory driver functions
int32_t directory_open(const uint8_t* filename);
int32_t directory_close(int32_t fd);
int32_t directory_write(int32_t fd, const void* buf, int32_t length);
int32_t directory_read(int32_t fd, void* buf, int32_t length);

// Test driver functions
// void test_open();

// void test_close();

// void test_wrtie();

// void test_file_driver();

#endif

