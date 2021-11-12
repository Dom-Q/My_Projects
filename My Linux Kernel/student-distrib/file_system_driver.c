
#include "file_system_driver.h"

// #include ""

// Global vars
// Working copy of directory entry
// Working indices for current file/directory
dentry_t d_entry;
unsigned int file_idx;
unsigned int directory_idx;

/* 
*  Function: file_open
*  Input: string filename
*  Output: Populate directory entry
*  Return value: 0 on success, -1 on failure
*  Side effects: None
*/
int32_t file_open(const uint8_t* filename){
    // Initialize file index
    // Fill respective directory entry
    file_idx = 0;
    return read_dentry_by_name(filename, &d_entry);
}

/* 
*  Function: file_close
*  Input: int fild descriptor
*  Output: None
*  Return value: 0
*  Side effects: None
*/
int32_t file_close(int32_t fd){
    return 0;
}

/* 
*  Function: file_write
*  Input: int fild descriptor, buffer buf, length of bytes to write
*  Output: None
*  Return value: -2
*  Side effects: None
*/
int32_t file_write(int32_t fd, const void* buf, int32_t length){
    return -1;
}

/* 
*  Function: file_read
*  Input: int fild descriptor, buffer = buf, length = # of bytes to read
*  Output: Buffer filled with read data
*  Return value: Number of bytres read
*  Side effects: None
*/
int32_t file_read(int32_t fd, void* buf, int32_t length){
    // Read data from file and store number of bytes read
    int read_bytes;
    read_bytes = read_data(d_entry.inode_num, file_idx, buf, length);

    if(read_bytes == -1) return 0;

    // Increment file index accordingly and return number of bytes read
    file_idx += read_bytes;
    return read_bytes;
}

/* 
*  Function: directory_open
*  Input: string filename
*  Output: Populated global dentry
*  Return value: 0
*  Side effects: None
*/
int32_t directory_open(const uint8_t* filename){
    directory_idx = 0;
    return read_dentry_by_name(filename, &d_entry);
}

/* 
*  Function: directory_close
*  Input: int file descriptor
*  Output: None
*  Return value: 0
*  Side effects: None
*/
int32_t directory_close(int32_t fd){
    return 0;
}

/* 
*  Function: file_write
*  Input: int file descriptor, buffer buf, length in bytes
*  Output: None
*  Return value: -1
*  Side effects: None
*/
int32_t directory_write(int32_t fd, const void* buf, int32_t length){
    return -1;
}

/* 
*  Function: directory_read
*  Input: int fild descriptor, buffer = buf, length = # of bytes to read
*  Output: Buffer filled with read filename
*  Return value: Number of bytes read
*  Side effects: None
*/
int32_t directory_read(int32_t fd, void* buf, int32_t length){
    // Populate directory entry based on idex
    int stat;
    stat = read_dentry_by_index(directory_idx, &d_entry);

    // Validate read
    if(stat == -1) return 0;

    // Increment directory index
    // Find length of name and return it
    directory_idx++;
    // int name_len = strlen(d_entry.file_name);
    int name_len = (strlen(d_entry.file_name) < FILENAME_SIZE) ? strlen(d_entry.file_name) : FILENAME_SIZE;

    strncpy(buf, d_entry.file_name, name_len);

    // strcpy(buf, d_entry.file_name);
    return name_len;
}

// Tests
/* 
*  Function: test_open
*  Input: None
*  Output: None
*  Return value: None
*  Side effects: Print output value of open functions
*/
void test_open(){
    printf("Test File Open:\n");
    printf("f_open returned %d\n", file_open("."));

    printf("Test Dir Open:\n");
    printf("f_open returned %d\n", directory_open("."));
}

/* 
*  Function: test_close
*  Input: None
*  Output: None
*  Return value: None
*  Side effects: Print output value of close functions
*/
void test_close(){
    printf("Test File Close:\n");
    printf("F_close returned %d\n", file_close(1));

    printf("Test Dir Close:\n");
    printf("f_open returned %d\n", directory_close(1));
}

/* 
*  Function: test_write
*  Input: None
*  Output: None
*  Return value: None
*  Side effects: Print output value of wrtie functions
*/
void test_write(){
    char buf[1];
    printf("Test File Write:\n");
    printf("F_close returned %d\n", file_write(1, buf, 0));

    printf("Test Dir Close:\n");
    printf("f_open returned %d\n", directory_write(1, buf, 0));  
}

/* 
*  Function: test_file_driver
*  Input: None
*  Output: None
*  Return value: None
*  Side effects: Print all driver tests
*/
void test_file_driver(){
    test_open();
    test_write();
    test_close();
}