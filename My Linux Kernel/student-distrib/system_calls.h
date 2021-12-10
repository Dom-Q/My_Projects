#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "types.h"
#include "file_system_driver.h"

#include "paging.h"
#include "x86_desc.h"
#include "std_driver.h"
#include "rtc.h"
#include "x86_desc.h"
#include "context_switch.h"

#define MAX_PROCESSES 6
extern uint32_t process[MAX_PROCESSES];

// File operations jump table
// All files support read, write, open, close
typedef struct file_operations
{
    int32_t (*read)(int32_t fd, void *buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void *buf, int32_t nbytes);
    int32_t (*open)(const uint8_t *filename);
    int32_t (*close)(int32_t fd);
} fops_table;

// File descriptor
// fojtp - fops table pointer
// inode - inode number associated with file
// file_position - data index within file
// flags - 1 if open, 0 if closed
typedef struct file_descriptor
{
    fops_table* fojtp;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
} file_descriptor;

// Process Control Block
// FDT - file descriptor table
// EIP - instruction pointer
// parent_pcb_num
// base_kernel_address - address of parent base kernel
// esp, ebp - old state of parent process
typedef struct process_control_block
{
    file_descriptor fdt[8];
    uint8_t eip[4];
    uint32_t parent_pcb_num;
    uint32_t base_kernel_addr;
    uint32_t esp;
    uint32_t ebp; 
    uint32_t process_num;

    uint32_t curr_esp;
    uint32_t curr_ebp;  
    
    // uint32_t esp0;
    // uint16_t ss0;
    
    int active;

    char arg[33];
    // esp, ebp, esp0
} pcb;

// PCB pointer to be referenced across contexts
extern pcb* global_pcb;

// Array of states of PCBs for each terminal
extern pcb global_procs[3]; // Up to 3 terminals

// Scheduler processing index
extern int running_proc;

extern int running_shell;

// Current displayed terminal
extern int displayed_terminal;

// System Call Headers
int8_t do_halt(uint8_t status);
int32_t do_execute(uint8_t *command);
int32_t do_read(int32_t fd, void *buf, int32_t nbytes);
int32_t do_write(int32_t fd, void *buf, int32_t nbytes);
int32_t do_open(const uint8_t *filename);
int32_t do_close(int32_t fd);
int32_t do_getargs(uint8_t *buf, int32_t nbytes);
int32_t do_vidmap(uint8_t **screen_start);

// Helper Functions
uint32_t check_if_executable(uint8_t *executable, uint8_t buffer[28]);
pcb *get_parent_address(uint32_t parent_id_num);

#endif
