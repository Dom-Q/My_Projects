#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "system_calls.h"
#include "lib.h"

#define FILENAME_SIZE 32
#define KB 1024
#define KB_4 4096

// Directory Entry Structure
typedef struct directory_entry{
    char file_name[FILENAME_SIZE];
    uint32_t file_type;
    uint32_t inode_num;
    uint8_t reserved[24]; // 24 bytes of padding to reach 64B
} dentry_t;

// Struct for data block
typedef struct data_block{
    uint8_t byte[4*KB];
} data_block_t;

// I-Node Structure
typedef struct i_node{
    uint32_t length; // number of bytes in file
    uint32_t data_block[KB - 1]; // 1024 4-byte indices, -1 for the first index storing the length
} i_node_t;

// Struct for superblock
typedef struct superblock{
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t num_dblocks;
    uint8_t reserved[52]; // 52 reserved bytes to reach 64B
    dentry_t d_entries[63]; // 63 remaining block sof 64B directory entries sums to 4 KB
} superblock;

// Initialize filesystem by acquiring address of Superblock
void init_filesystem(uint32_t filesystem_base);

// FileSystem API
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// Helper Functions
i_node_t* get_inode_base(superblock* sb_base);
data_block_t* get_dblock_base(superblock* sb_base);

// // Test functions
// void print_filenames();
// void test_file_read();

#endif
