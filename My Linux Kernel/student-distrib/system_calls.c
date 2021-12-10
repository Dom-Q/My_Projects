#include "system_calls.h"

// #include "exceptions.h"

#define ONE28_MB 0x08000000
#define EIGHT_MB 0x00800000
#define FOUR_MB 0x00400000
#define EIGHT_KB 0x00002000
#define NUM_STD_PROCESSES 2
#define TOTAL_NUM_PROCESSES (MAX_PROCESSES + NUM_STD_PROCESSES)
#define NAME_LENGTH 32
#define NUM_IMPORTANT_BYTES 28
#define NUM_MAX_TYPED 128
#define NUM_EIP_VALS 4
#define PAGE_OFFSET 0x00048000
#define RTC_FILE_TYPE 0
#define DIR_FILE_TYPE 1
#define DATA_FILE_TYPE 2
#define NO_MORE_PROCESSES 2
#define BASE_SHELLS_PROCS 2
#define MAX_PROCESSES 6
#define NUM_TERMINALS 3
#define MAX_SHELLS_PER_TERM 4
#define EMPTY_SHELL_PROC -2

extern int exception;

uint8_t *cur_command;

pcb global_procs[NUM_TERMINALS];

pcb *global_pcb;

// Active processes
uint32_t process[MAX_PROCESSES] = {0};

int displayed_terminal = 0;
int running_shell = 0;
int running_proc = 0;


// Return val from halt to be referenced across contexts
uint32_t halt_rval;

// Jump tables //
// ----------- //

// STD_IN
fops_table std_in_ops = {stdin_read, stdin_write, stdin_open, stdin_close};
// STD_OUT
fops_table std_out_ops = {stdout_read, stdout_write, stdout_open, stdout_close};
// RTC
fops_table rtc_ops = {rtc_read, rtc_write, rtc_open, rtc_close};
// Directory
fops_table directory_ops = {directory_read, directory_write, directory_open, directory_close};
// Data file
fops_table data_file_ops = {file_read, file_write, file_open, file_close};

static int shells_in_terms[NUM_TERMINALS][MAX_SHELLS_PER_TERM] = {{EMPTY_SHELL_PROC, EMPTY_SHELL_PROC, EMPTY_SHELL_PROC, EMPTY_SHELL_PROC},
                                                                  {EMPTY_SHELL_PROC, EMPTY_SHELL_PROC, EMPTY_SHELL_PROC, EMPTY_SHELL_PROC},
                                                                  {EMPTY_SHELL_PROC, EMPTY_SHELL_PROC, EMPTY_SHELL_PROC, EMPTY_SHELL_PROC}};

/* 
*  Function: get_parent_address
*  Input: pointer to a pcv
*  Output: None
*  Return value: pointer to parent process inode
*  Side effects: None
*/
pcb *get_parent_address(uint32_t parent_id_num)
{
    return (pcb *)(EIGHT_MB - ((parent_id_num + 1) * EIGHT_KB)); // 8 MB - num_processes * 2KB per process
}

/* 
*  Function: do_halt
*  Input: status variable to return to parent
*  Output: None
*  Return value: None 
*  Side effects: Switches context back to parent process, restores state
*/
int8_t do_halt(uint8_t status)
{
    // Check edge case if halt called on original shells (processes 0, 1, or 2)
    cli();

    if (global_procs[running_shell].process_num <= BASE_SHELLS_PROCS)
    {
        // Clear process and recall shell
        process[global_procs[running_shell].process_num] = 0;
        uint8_t shell_name[] = "shell0";
        shell_name[5] = '0' + global_procs[running_shell].process_num;
        do_execute(shell_name);
    }
    // if there is more than 1 recursive shell and the process being halted is a recursive shell
    // if (status == 0 && num_recursive_shells > 0 && (int)recursive_shells_arr[num_recursive_shells - 1] == (int)global_pcb->process_num)
    // {
    //     if (num_recursive_shells > 0)
    //     {
    //         num_recursive_shells--;
    //         recursive_shells_arr[num_recursive_shells] = -2;
    //     }
    // }

    int i;
    // for (i = 0; i < MAX_SHELLS_PER_TERM; i++)
    // {
    //     if (shells_in_terms[displayed_terminal][i] == global_pcb->process_num)
    //     {
    //         shells_in_terms[displayed_terminal][i] = EMPTY_SHELL_PROC;
    //         break;
    //     }
    // }

    // Free process slot
    process[global_procs[running_shell].process_num] = 0;

    // Close files all files in child process pcb
    for (i = NUM_STD_PROCESSES; i < NUM_STD_PROCESSES + MAX_PROCESSES; i++)
    {
        do_close(i);
    }

    // Temporarily store parent esp/ebp
    uint32_t esp_ = global_procs[running_shell].esp;
    uint32_t ebp_ = global_procs[running_shell].ebp;
    int page_idx = NAME_LENGTH;

    //physical memory offset for paging
    // Update global pcb pointer and set tess params for another switch
    // tss.esp0 = global_pcb->base_kernel_addr;
    
    tss.esp0 = EIGHT_MB - ((global_procs[running_shell].parent_pcb_num + 1)*EIGHT_KB) - 4;
    uint32_t physical_address = EIGHT_MB + (global_procs[running_shell].parent_pcb_num * FOUR_MB); // start at 8MB, 4MB per page
    page_dir[page_idx] = physical_address | 0x87;                                                       //10000111: setting extra bits for pde

    // user_page_tab[0] = 0;                                                               //resetting vidmap
    // page_dir[(MB_256 / MB_4)] = 0;                                                      //resetting vidmap

    tss.ss0 = KERNEL_DS;

    flush_tlb();
    global_pcb = get_parent_address(global_procs[running_shell].parent_pcb_num);

    global_procs[running_shell] = *global_pcb;
    if (global_procs[running_shell].process_num > 2)
    {
        running_proc = global_procs[running_shell].parent_pcb_num;
    }
    else
    {
        running_proc = global_procs[running_shell].process_num;
    }

    // else{
    //     // recursively find parent
    // }

    // Assign return val
    if (exception == 1)
    {
        halt_rval = 256; // if exception raised, return 256
        exception = 0;
    }
    else
        halt_rval = status;

    // sti();
    // Restore old state and jump to old execute call
    asm volatile("\n\
        movl %0, %%esp \n\
        movl %1, %%ebp \n\
        jmp execute_ret;"
                 :
                 : "r"(esp_), "r"(ebp_));

    return halt_rval;
}

/* 
*  Function: do_exectute
*  Input: uint8_t* - command file name
*  Output: None
*  Return value: -1 on failure, 0-255 on success, 256 for unknown error 
*  Side effects: Starts a new process and switches context to it
*/
int32_t do_execute(uint8_t *command)
{
    cli();
    int proc_is_shell = 0;
    if (command == NULL)
        return -1;

    int i = 0;
    int j = 0;
    // int k;
    int process_num = 3; // 0-2 are reserved for shells in each terminal, so new process num will be at least 3

    uint32_t eip_;
    int shell_length = 5;
    int shell_num_length = 6;
    uint8_t *shell = (uint8_t *)"shell";
    uint8_t *shell0 = (uint8_t *)"shell0";
    uint8_t *shell1 = (uint8_t *)"shell1";
    uint8_t *shell2 = (uint8_t *)"shell2";

    // check which process availible and take it.
    if (strncmp((int8_t *)command, (int8_t *)shell0, shell_num_length) == 0)
    {
        command = shell;
        if (process[0] == 0) // if free
        {
            process_num = 0;
            process[process_num] = 1;
            shells_in_terms[process_num][0] = process_num;
        }
        else
            return 0;
    }
    else if (strncmp((int8_t *)command, (int8_t *)shell1, shell_num_length) == 0)
    {
        command = shell;
        if (process[1] == 0) // if free
        {
            process_num = 1;
            process[process_num] = 1;
            shells_in_terms[process_num][0] = process_num;
        }
        else
            return 0;
    }
    else if (strncmp((int8_t *)command, (int8_t *)shell2, shell_num_length) == 0)
    {
        command = shell;
        if (process[2] == 0) // if free
        {
            process_num = 2;
            process[process_num] = 1;
            shells_in_terms[process_num][0] = process_num;
        }
        else
            return 0;
    }
    else
    {
        while (process_num < MAX_PROCESSES)
        {
            if (process[process_num] == 0) // if free
            {
                process[process_num] = 1;
                break;
            }
            process_num++;
        }
        if (process_num >= MAX_PROCESSES)
            return NO_MORE_PROCESSES;

        if (strncmp((int8_t *)command, (int8_t *)shell, shell_length) == 0)
        {
            int i;
            for (i = 0; i < MAX_SHELLS_PER_TERM; i++)
            {
                if (shells_in_terms[displayed_terminal][i] == EMPTY_SHELL_PROC)
                {
                    shells_in_terms[displayed_terminal][i] = process_num;
                    proc_is_shell = process_num;
                    break;
                }
            }
        }
    }

    //uint8_t * exec_name[NAME_LENGTH + 1] = {'\0'}; // 1 is for endline char
    uint8_t exec_name[NAME_LENGTH + 1] = {'\0'}; // 1 is for endline char
    char arg[NAME_LENGTH + 1] = {'\0'};          // 1 is for endline char

    //iterate through command until first non space is found
    while (i < MAX_CHARS && (char)(command[i]) == ' ')
    {
        i++;
    }

    // parse until next " " to get filename
    while (i < MAX_CHARS && (char)command[i] != ' ' && (char)command[i] != '\0' && j < NAME_LENGTH)
    {
        exec_name[j] = (char)command[i];
        i++;
        j++;
    }

    exec_name[j] = '\0';
    if (j == NAME_LENGTH)
    {
        while (i < MAX_CHARS && (char)command[i] != ' ')
        {
            i++;
        }
    }

    //chance that this is brocken in case that j name length is hit
    // Add null char to file name to make proper string

    i++;
    if (command[i] != '\0')
    {
        //iterate through command until second non space is found
        while (i < MAX_CHARS && (char)(command[i]) == ' ')
        {
            i++;
        }
        // parse until next " " to get arg
        j = 0;
        while (i < MAX_CHARS && (char)command[i] != ' ' && j < NAME_LENGTH)
        {
            arg[j] = (char)command[i];
            i++;
            j++;
        }
        // Add null char to arg to make proper string
        arg[j] = '\0';
    }

    //check if file is executable return the data is it is if null return
    uint8_t executable_buffer[FILENAME_SIZE]; // 28 - max filesize
    if (check_if_executable(exec_name, executable_buffer) == -1)
    {
        process[process_num] = 0;
        return -1;
    }

    //get eip from buffer vals 24-27
    uint8_t eip[NUM_EIP_VALS];

    for (i = 0; i < 4; i++)
    {
        eip[i] = executable_buffer[NUM_IMPORTANT_BYTES - (i + 1)]; // 1 accounts for 0 indexing
    }

    // PDE mapping user program to 0x800000
    int page_idx = FILENAME_SIZE;

    //physical memory offset for paging
    uint32_t physical_address = EIGHT_MB + (process_num * FOUR_MB);
    // uint32_t physical_address = 0x00800000;
    //create pcb
    //set values for eip, base kernel address and first to file descriptors of the fdt
    page_dir[page_idx] = (physical_address | 0x87); //01010111

    // flush tlb
    flush_tlb();

    // Create current process PCB
    pcb *curr_pcb;
    // Determine address of pcb
    curr_pcb = (pcb *)(0x00800000 - ((process_num + 1) * 0x00002000)); // start at 8MB, 4B per page
    curr_pcb->process_num = process_num;

    // Populate PCB data //
    // ----------------- //
    // Assign kernel base address
    curr_pcb->base_kernel_addr = EIGHT_MB - ((process_num)*EIGHT_KB) - 4; // -0x4 maybe 8MB - (process_num)*8kb - 4 to get to base of stack

    // Start location for instruction pointer
    for (i = 0; i < 4; i++)
    {
        curr_pcb->eip[i] = eip[i];
    }
    // Assign parent process num
    //FIX THIS

    //curr_pcb->parent_pcb_num = process_num - 1;
    if (process_num <= 2)
    {
        curr_pcb->parent_pcb_num = -1;
    }
    else
    {
        curr_pcb->parent_pcb_num = running_proc; // BECOMES SCHEDULE IDX
    }
    // else if (proc_is_shell != 0)
    // {
    //     int i;
    //     for (i = 0; i < MAX_SHELLS_PER_TERM; i++)
    //     {
    //         if (shells_in_terms[displayed_terminal][i] == proc_is_shell)
    //         {
    //             curr_pcb->parent_pcb_num = shells_in_terms[displayed_terminal][i - 1];
    //             break;
    //         }
    //     }
    // }
    // else
    // {
    //     int i;
    //     for (i = 0; i < 4; i++)
    //     {
    //         if (shells_in_terms[displayed_terminal][i] == -2)
    //         {
    //             curr_pcb->parent_pcb_num = shells_in_terms[displayed_terminal][i - 1];
    //             break;
    //         }
    //     }
    // }

    // Fill file operation table
    // STD IN and OUT guranteed
    file_descriptor std_in_desc;
    file_descriptor std_out_desc;

    // STD IN descriptor data
    std_in_desc.flags = 1;
    std_in_desc.inode = 0;
    std_in_desc.file_position = 0;
    std_in_desc.fojtp = &std_in_ops;

    // STD OUT
    std_out_desc.flags = 1;
    std_out_desc.inode = 0;
    std_out_desc.file_position = 0;
    std_out_desc.fojtp = &std_out_ops;

    // Assign std descriptors
    curr_pcb->fdt[0] = std_in_desc;
    curr_pcb->fdt[1] = std_out_desc;

    // Get EIP of next context
    // bit-shift values to get the correct eip from the bytes taking into account little-endian
    eip_ = ((eip[0] << 24) | (eip[1] << 16) | (eip[2] << 8) | (eip[3]));

    // Save current state (ESP and EBP)
    asm volatile("\n\
        movl %%esp, %0 \n\
        movl %%ebp, %1"
                 : "=r"(curr_pcb->esp), "=r"(curr_pcb->ebp));

    memcpy(curr_pcb->arg, arg, FILENAME_SIZE + 1);

    // If initializing new shell

    //
    if (process_num <= 2)
    {
        // Save displayed terminal state
        /*
        // asm volatile("\n\
        //     movl %%esp, %0 \n\
        //     movl %%ebp, %1"
        //              : "=r"(curr_pcb->curr_esp), "=r"(curr_pcb->curr_ebp));
        */
        displayed_terminal = process_num;
        // curr_pcb->active = 1;
        // global_procs[running_shell] = *curr_pcb;
    }
    // else{
    /*
    //     // asm volatile("\n\
    //     //     movl %%esp, %0 \n\
    //     //     movl %%ebp, %1"
    //     //             : "=r"(curr_pcb->curr_esp), "=r"(curr_pcb->curr_ebp));
    //     // // curr_pcb->active = 1;
    //     // global_procs[displayed_terminal] = *curr_pcb;
        */
    // }
    // Update global pcb
    running_proc = process_num;
    global_procs[running_shell] = *curr_pcb;
    // global_pcb = &global_procs[displayed_terminal];
    global_pcb = curr_pcb;
    //read the file into memory at physical address location
    int32_t curr_fd = do_open(exec_name);
    do_read(curr_fd, (void *)(ONE28_MB | PAGE_OFFSET), 1048576); // Maximum of 1 MB
    do_close(curr_fd);

    // switch context
    // Set ring level and new stack base
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - ((process_num + 1) * EIGHT_KB) - 4; // 8 MB - num_processes * 2KB per process
    sti();
    context_switch(eip_);
    asm volatile("execute_ret:");
    sti();
    return halt_rval;
}

/* 
*  Function: check_if_executable
*  Input: uint8_t* executable - executable filename
          uint8_t buffer - output buffer
*  Output: Fill buffer with EIP and executable magic num
*  Return value: 1 on sucess, -1 on failute
*  Side effects: Starts a new process and switches context to it
*/
uint32_t check_if_executable(uint8_t *executable, uint8_t buffer[FILENAME_SIZE])
{
    if (executable == NULL || buffer == NULL)
        return -1;
    dentry_t dentry;
    // Validate/acquire dir entry block
    if (read_dentry_by_name(executable, &dentry) == -1)
    {
        return -1;
    }

    // if(file_open(executable) == -1){
    //     return NULL;
    // }
    // uint32_t inode_idx = dentry.inode_num;
    // 28 bytes acquired - first 4 determine if executable, last 4 represent starting address of program
    uint32_t file_size = NUM_IMPORTANT_BYTES;
    read_data(dentry.inode_num, 0, buffer, file_size);
    if (buffer[0] == 0x7F && buffer[1] == 0x45 && buffer[2] == 0x4c && buffer[3] == 0x46) // provided magic numbers for executable
    {
        return 1;
    }
    return 0;
}

/* 
*  Function: do_read
*  Input: 
    - int32_t fd - file descriptor num
    - buf - buf to store input data
    - nbytes - number of bytes to be read
*  Output: None
*  Return value: -1 on failure, otherwise number of bytes read from file
*  Side effects: fill buffer with data from file
*/
int32_t do_read(int32_t fd, void *buf, int32_t nbytes)
{
    if (buf == NULL)
        return -1;
    // Validate args and pcb objects
    if ((fd >= 0 && fd < TOTAL_NUM_PROCESSES) && global_pcb->fdt[fd].flags == 1 && global_pcb->fdt[fd].fojtp->read != NULL)
    {
        // Call morphed read function, return number of bytes read
        int32_t read_bytes = global_pcb->fdt[fd].fojtp->read(fd, buf, nbytes);
        return read_bytes;
    }
    return -1;
}

/* 
*  Function: do_write
*  Input: 
    - int32_t fd - file descriptor num
    - buf - buf to store output data
    - nbytes - number of bytes written
*  Output: None
*  Return value: -1 on failure, otherwise number of bytes wrriten to file
*  Side effects: fill terminal buffer with data from file
*/
int32_t do_write(int32_t fd, void *buf, int32_t nbytes)
{
    if (buf == NULL)
        return -1;
    // Validate args and pcb objects
    if ((fd >= 0 && fd < TOTAL_NUM_PROCESSES) && global_pcb->fdt[fd].flags == 1 && global_pcb->fdt[fd].fojtp->write != NULL)
    {
        // Call morphed write function, return number of bytes written
        return global_pcb->fdt[fd].fojtp->write(fd, buf, nbytes);
    }
    return -1;
}

/* 
*  Function: do_open
*  Input: 
    - uint8_t* filename - name of file to open
*  Output: None
*  Return value: -1 on failure, fd of opened file on success
*  Side effects: None
*/
int32_t do_open(const uint8_t *filename)
{
    if (!filename)
        return -1;

    dentry_t dentry;
    // read and validate directory entriy by filename
    if (read_dentry_by_name(filename, &dentry) != -1)
    {
        int i;
        // go through fdt to find first file opening
        for (i = NUM_STD_PROCESSES; i < TOTAL_NUM_PROCESSES; i++)
        {
            if (global_pcb->fdt[i].flags == 0)
            { // once an open file space is found in the fdt
                // Populate file descriptor
                file_descriptor curr_fd;
                curr_fd.flags = 1;         // Set file as open
                curr_fd.file_position = 0; // Initialize file position at start

                // Switch based on file type
                // Assign proper function tables based on file type
                switch (dentry.file_type)
                {
                // RTC file type
                case 0:
                {
                    curr_fd.fojtp = &rtc_ops;
                    curr_fd.inode = 0;
                    break;
                }
                // Directory file type
                case 1:
                {
                    curr_fd.fojtp = &directory_ops;
                    curr_fd.inode = 0;
                    break;
                }
                // Regular data file type
                case 2:
                {
                    curr_fd.fojtp = &data_file_ops;
                    curr_fd.inode = dentry.inode_num;
                    break;
                }
                // Invalid file type
                default:
                    return -1;
                }
                // Populate PCD and call proper open
                // Return file descriptor on success
                global_pcb->fdt[i] = curr_fd;
                global_pcb->fdt[i].fojtp->open(filename);
                return i;
            }
        }
    }
    return -1;
}

/* 
*  Function: do_close
*  Input: 
    - int32_t fd - file descriptor num
*  Output: None
*  Return value: -1 on failure, 0 on successful close
*  Side effects: close file and free index in file descriptor table
*/
int32_t do_close(int32_t fd)
{
    // Validate file descriptor and its members
    if ((fd >= 2 && fd < TOTAL_NUM_PROCESSES) && global_pcb->fdt[fd].flags == 1 && global_pcb->fdt[fd].fojtp->close != NULL)
    {
        // Check if pcb->flags = 0 (1 for opened 0 for closed), assign 0 if opened
        global_pcb->fdt[fd].flags = 0;
        return global_pcb->fdt[fd].fojtp->close(fd);
    }
    return -1;
}

/* 
*  Function: do_getargs
*  Input: 
    - uint8_t *buf: array to fill with the arg
    - int32_t nbytes: how many bytes are put into the arg
*  Output: None
*  Return value: -1 on failure, 0 on successful getarg
*  Side effects: puts arg in pointer buffer to be used by user programs
*/
int32_t do_getargs(uint8_t *buf, int32_t nbytes)
{
    if (!buf)
        return -1;

    // if length of buf is longer than bytes to be read, ret -1
    // int len = strlen((char *)buf);
    // if (len > nbytes)
    // if(global_pcb->arg[0] == '\0') return -1;

    // add the arg to the buf
    int i = 0;
    while (i < nbytes && global_pcb->arg[i] != '\0')
    {
        buf[i] = global_pcb->arg[i];
        i++;
    }
    // null terminal buf
    if (i > 32)
        return -1;

    buf[i] = '\0';
    return 0;
}

/* 
*   do_vidmap
*  Input:           **screen_pointer - pointer to virtual user memory
*  Output:          none
*  Return value:    -1 on failure, 0 on successful close
*  Side effects:    adds a page table with correct entry to PD and 
*                   writes virtual video memory address to the passed pointer
*/
int32_t do_vidmap(uint8_t **screen_start)
{
    //check if pointer is valid
    if (((uint32_t)screen_start == NULL) || ((uint32_t)screen_start == MB_4))
    {
        return -1;
    }
    //assign a page table and write it to page directory
    if(running_shell == displayed_terminal){
        user_page_tab[0] = (VID_MEM_ADDR | US_RW_P);
    }
    else{
        user_page_tab[0] = ((VID_MEM_ADDR + (running_shell + 1) * KB_4)| US_RW_P);
    }
    page_dir[(MB_256 / MB_4)] = ((uint32_t)user_page_tab | US_RW_P);

    flush_tlb();

    //set the pointer to the new virtual video memory address
    *screen_start = (uint8_t *)MB_256;

    return 0;
}
