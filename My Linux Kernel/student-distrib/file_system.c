
#include "file_system.h"
// #include "file_system_driver.h"

// Super/Boot block pointer
superblock* S;

/* 
*  Function: init_filesystem
*  Input: 32 bit base address of superblock
*  Output: None
*  Return value: None
*  Side effects: Populate superblock structure
*/
void init_filesystem(uint32_t filesystem_base){
    // Store base address of superblock
    S = (superblock*)filesystem_base;
}

/* 
*  Function: read_dentry_by_name
*  Input: Filename, Pointer to dentry to be updated
*  Output: Data for directory entry with given filename
*  Return value: 0 on success, -1 on failure
*  Side effects: None
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    // Validate filename
    unsigned int i;
    unsigned int num_entries = S->num_dentries;
    unsigned int len = strlen((const int8_t*)fname);
    if(len > 32 || len == 0) return -1;

    // Search for d_entry by name
    // Iterate through entries
    for(i = 0; i < num_entries; i++){
        // Compare names
        int cmp_val;

        // Compare names by length before 
        unsigned int curr_len = strlen(S->d_entries[i].file_name);
        if(curr_len != len) continue;

        cmp_val = strncmp((const int8_t*)(S->d_entries[i].file_name), fname, len);
        
        // Evaluate comparison
        if(cmp_val == 0){
            *dentry = S->d_entries[i];
            return 0;
        }
    }
    // -1 on failure - name not found
    return -1;
}

/* 
*  Function: read_dentry_by_name
*  Input: Dentry index, Pointer to dentry to be updated
*  Output: Data for directory entry with given index
*  Return value: 0 on success, -1 on failure
*  Side effects: None
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    // Validate index
    if(index < 0 || index >= S->num_dentries) return -1;
    // Search by inode
    *dentry = S->d_entries[index];
    return 0;
}

/* 
*  Function: get_inode_base
*  Input: Base address of superblock
*  Output: None
*  Return value: Address of first Inode block
*  Side effects: None
*/

i_node_t* get_inode_base(superblock* sb_base){
    i_node_t* ret_val = (i_node_t*) ((uint8_t*)sb_base + (4*KB)); // 4KB after the superblock is the start of the inode list
    return ret_val;
}

data_block_t* get_dblock_base(superblock* sb_base){
    // Calculate starting block of data blocks (superblock + num indoes)
    unsigned int num_blocks = 1 + sb_base->num_inodes;
    data_block_t* start_addr = (data_block_t*) ( (uint8_t*)sb_base + (KB_4*num_blocks) );
}

/* 
*  Function: read_data
*  Input: 
*       - inode number
*       - offset in file
*       - ending byte to read
*  Output: 
*       - buffer with fetched data from file
*  Return value: 
*       - number of bytes read
*       - -1 on failure
*  Side effects: None
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    unsigned int num_dblocks;
    unsigned int i;
    unsigned int read_bytes;
    unsigned int total_bytes;
    unsigned int buf_idx;
    // Get Base of inode list
    i_node_t* inodes = get_inode_base(S);

    // Validate inode and fetch
    if(inode >= S->num_inodes) return -1;
    i_node_t curr_node = inodes[inode];

    // Quick bounds check
    if(offset > curr_node.length){
        return -1;
    }

    // Calculate number of data blocks for given inode
    num_dblocks = ( curr_node.length / KB_4 ) + 1;
    
    // Find start position
    i = 0;
    read_bytes = 0;
    total_bytes = 0;
    while(offset >= (KB_4)){
        i++;
        offset -= (KB_4);
    }

    // subtract from total blocks to iterate through
    // Set indices for buffer, file
    num_dblocks -= i;
    total_bytes = KB_4*i;
    buf_idx = 0;

    // Iterate through data blocks
    for(i = i; i < num_dblocks; i++){
        // Validate pointer
        if(curr_node.data_block[i] == NULL) return -1;

        unsigned int file_pos;
        // Get current data block address
        data_block_t* start_block = get_dblock_base(S);
        data_block_t* curr_block = (data_block_t*)start_block + curr_node.data_block[i];

        // Check if there is remaining offset in current block
        // Clear offset if there is any - only indents into the first data block
        file_pos = offset;
        total_bytes += file_pos;
        offset = 0;

        // Read data
        // Fill buffer
        // Conditions - length not reached and within bounds of current block
        while(read_bytes < length && file_pos < KB_4 && total_bytes < curr_node.length){
            buf[buf_idx] = curr_block->byte[file_pos];
            buf_idx++;
            file_pos++;
            read_bytes++;
            total_bytes++;
        }
    }
    
    // Ending byte should be length
    return read_bytes;
}

// Testing
/* 
*  Function: read_dentry_by_name
*  Input: None
*  Output: None
*  Return value: None
*  Side effects: Print out file names
*/
void print_filenames(){
    // Iterate through directrory entries and print filenames
    int i;
    printf("All files in filesystem:\n");

    // Initialize a buffer to hold filename
    char name_buf[FILENAME_SIZE];

    // Loop through directory entries and read names
    while(directory_read(0, name_buf, FILENAME_SIZE) != 0){
        printf("%s\n", name_buf);
    }
}

/* 
*  Function: test_file_read
*  Input: None
*  Output: None
*  Return value: None
*  Side effects: Read data from txt file
*/

void test_file_read(){
    int size;
    // size = 5349;
    // size = 198;
    // uint8_t data_buf[198];
    // file_open("hello");
    // file_open("frame0.txt");
    // file_read(0, data_buf, size); 

    dentry_t dentry;
    read_dentry_by_name((int8_t*)"hello", &dentry);
    uint32_t inode_idx = dentry.inode_num;

    uint32_t file_size = ((i_node_t*)(S + (inode_idx + 1)))->length;
    uint8_t buffer[file_size];
    printf("buffer length %d\n", file_size);
    file_open("hello");
    
    printf("file_size %d\n",  file_read(0, &buffer, file_size));
    // while(file_read(0, data_buf, 5349) != 0){
    //     //printf("%s\n\n", data_buf);
    //     break;
    // }
    int i;
    for (i = 0; i < file_size; i++){
        // putc(data_buf[i]);
        if (buffer[i] == 0 || buffer[i] == 1) {
            continue;
        }
        putc(buffer[i]);
    }
    return;
}
