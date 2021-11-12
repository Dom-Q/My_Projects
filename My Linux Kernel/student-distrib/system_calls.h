#include "types.h"
#include "file_system_driver.h"

#include "paging.h"
#include "x86_desc.h"
#include "std_driver.h"
#include "rtc.h"
#include "x86_desc.h"
#include "context_switch.h"

// File operations jump table
typedef struct file_operations{
    int32_t (*read)(int32_t fd, void *buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void *buf, int32_t nbytes);
    int32_t (*open)(const uint8_t *filename);
    int32_t (*close)(int32_t fd);
} fops_table;

// File descriptor
typedef struct file_descriptor{
    fops_table *fojtp;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
} file_descriptor;

// Process Control Block
typedef struct process_control_block{
    file_descriptor fdt[8];
    uint8_t eip[4];
    uint32_t parent_pcb_num;
    uint32_t base_kernel_addr;
    uint32_t esp;
    uint32_t ebp;
    // esp, ebp, esp0
} pcb;

int32_t do_halt(uint8_t status);
int32_t do_execute(uint8_t *command);
int32_t do_read(int32_t fd, void *buf, int32_t nbytes);
int32_t do_write(int32_t fd, void *buf, int32_t nbytes);
int32_t do_open(const uint8_t* filename);
int32_t do_close(int32_t fd);
uint32_t check_if_executable(uint8_t *executable, uint8_t buffer[28]);
pcb* get_parent_address(uint32_t parent_id_num);
// Helper Functions
uint32_t check_if_executable(uint8_t *executable, uint8_t buffer[28]);