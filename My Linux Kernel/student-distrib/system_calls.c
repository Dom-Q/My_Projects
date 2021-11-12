#include "system_calls.h"

// Active processes
uint32_t process[6] = {0, 0, 0, 0, 0, 0};

pcb *global_pcb;

// Jump tables //
// ----------- //

// STD_IN
fops_table std_in_ops = {stdin_read, stdin_write, stdin_open, stdin_close}; // read unfinished (terminal)
// STD_OUT
fops_table std_out_ops = {stdout_read, stdout_write, stdout_open, stdout_close}; // write unfinished (keyboard)
// RTC
fops_table rtc_ops = {rtc_read, rtc_write, rtc_open, rtc_close}; // These need to be given proper parameters
// Directory
fops_table directory_ops = {directory_read, directory_write, directory_open, directory_close};
// Data file
fops_table data_file_ops = {file_read, file_write, file_open, file_close};

pcb* get_parent_address(uint32_t parent_id_num){
    return (0x00800000 - ((parent_id_num + 1) * 0x00002000));
}

uint32_t clean_up_pcb(pcb* pcb_){
    //clean up file descriptors
    int i;
    for(i = 2; i < 8; i ++){

    }
    return 0;
}

uint32_t clean_up_file_descriptor(file_descriptor* fd){

return 0;
}

int32_t do_halt(uint8_t status)
{
    //get parent pcb address
    //pcb* parent_pcb = (get_parent_address(global_pcb->parent_pcb_num));
    //clean up pcb and file descriptor
   
    //change to parent pcb
    //add case for parent = -1
    if(global_pcb->parent_pcb_num == -1){
        process[0] = 0;
        do_execute("shell");
    }

    uint32_t esp_ = global_pcb->esp;
    uint32_t ebp_ = global_pcb->ebp;
    clean_up_pcb(global_pcb);

    asm volatile("\n\
        movl %0, %%esp \n\
        movl %1, %%ebp \n\
        jmp execute_ret;"
        :
        : "r"(esp_), "r"(ebp_)
    );
    return 1; //not sure what it should return
    //what shoulkd we return infrom halt for execute
    //how to clean up fd and pcbs
    //resetup paging?
    //if we have fd array in pcb and we switch psb will the file descriptor array be fine
    //ask about terminal read and if the way we have buffer will cause issue

}

int32_t do_execute(uint8_t *command)
{
    int i = 0;
    int j = 0;
    int k;
    int process_num = 0;
    uint32_t eip_;

    //check which process availible and take it.
    while (process_num < 6)
    {
        if (process[process_num] == 0)
        {
            process[process_num] = 1;
            break;
        }
        process_num++;
    }

    // No more space for new process
    if (process_num == 6)
    {
        return -1;
    }

    //iterate through command until first non space is found
    uint8_t exec_name[33];
    while ((char)(command[i]) == ' ')
    {
        i++;
    }

    // parse until next " " to get filename
    while ((char)command[i] != ' ' && j < 32)
    {
        exec_name[j] = command[i];
        i++;
        j++;
    }
    // Add null char to file name to make proper string
    exec_name[j] = '\0';

    //check if file is executable return the data is it is if null return
    uint8_t executable_buffer[28];
    check_if_executable(exec_name, executable_buffer);
    if (executable_buffer == NULL)
    {
        return -1;
    }

    //get eip from buffer vals 24-27
    uint8_t eip[4];
    eip[0] = executable_buffer[27];
    eip[1] = executable_buffer[26];
    eip[2] = executable_buffer[25];
    eip[3] = executable_buffer[24];

    //parse through remaining command string to get args 3 possible
    uint8_t args[3][128];
    for (k = 0; k < 3; k++)
    {
        while (command[i] == ' ')
        {
            i++;
        }
        j = 0;
        while (command[i] != ' ' && command[i] != '\0' && j < 128)
        {
            args[k][j] = command[i];
            i++;
            j++;
        }
    }

    // PDE mapping user program to 0x800000
    int page_idx = 32;

    //physical memory offset for paging
    uint32_t physical_address = 0x00800000 + (process_num * 0x00400000);
    // uint32_t physical_address = 0x00800000;
    //create pcb
    //set values for eip, base kernel address and first to file descriptors of the fdt
    page_dir[page_idx] = physical_address | 0x87; //01010111

    // flush tlb
    flush_tlb();
    //read the file into memory at physical address location
    file_open(exec_name);
    file_read(0, (void *)(0x08000000 | 0x00048000), 0x00400000);
    // Create current process PCB
    pcb *curr_pcb;
    // Determine address of pcb
    curr_pcb = (pcb *)(0x00800000 - ((process_num + 1) * 0x00002000));

    // Populate PCB data //
    // ----------------- //
    // Assign kernel base address
    curr_pcb->base_kernel_addr = 0x00800000 - ((process_num)*0x00002000); // -0x4 maybe 8MB - (process_num)*8kb - 4 to get to base of stack

    // Start location for instruction pointer
    curr_pcb->eip[0] = eip[0];
    curr_pcb->eip[1] = eip[1];
    curr_pcb->eip[2] = eip[2];
    curr_pcb->eip[3] = eip[3];

    // Assign parent process num
    curr_pcb->parent_pcb_num = process_num - 1;

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

    // Assign global pcb
    global_pcb = curr_pcb;

    // Check other file statuses
    eip_ = ((eip[0] << 24) | (eip[1] << 16) | (eip[2] << 8) | (eip[3]));

    asm volatile("\n\
        movl %%esp, %0 \n\
        movl %%ebp, %1"
        : "=r"(global_pcb->esp), "=r"(global_pcb->ebp)
    );

    //switch context
    context_switch(eip_);
    return 0;
}

uint32_t check_if_executable(uint8_t *executable, uint8_t buffer[28])
{
    dentry_t dentry;
    if (read_dentry_by_name(executable, &dentry) == -1)
    {
        return 0;
    }

    // if(file_open(executable) == -1){
    //     return NULL;
    // }
    // uint32_t inode_idx = dentry.inode_num;
    // 28 bytes acquired - first 4 determine if executable, last 4 represent starting address of program
    uint32_t file_size = 28;
    // uint8_t buffer[28];
    file_open(executable);
    file_read(0, buffer, file_size);
    if (buffer[0] == 0x7F && buffer[1] == 0x45 && buffer[2] == 0x4c && buffer[3] == 0x46)
    {
        return 1;
    }
    return 0;
}

int32_t do_read(int32_t fd, void *buf, int32_t nbytes)
{
    if ((fd >= 0 && fd < 8) && global_pcb->fdt[fd].flags == 1 && global_pcb->fdt[fd].fojtp->read != NULL)
    {
        int32_t read_bytes = global_pcb->fdt[fd].fojtp->read(fd, buf, nbytes);
        
    }
    return -1;
}

int32_t do_write(int32_t fd, void *buf, int32_t nbytes)
{
    if ((fd >= 0 && fd < 8) && global_pcb->fdt[fd].flags == 1 && global_pcb->fdt[fd].fojtp->write != NULL)
    {
        return global_pcb->fdt[fd].fojtp->write(fd, buf, nbytes);
    }
    return -1;
}

int32_t do_open(const uint8_t *filename)
{
    if(!filename) return -1;

    dentry_t dentry;
    if (read_dentry_by_name(filename, &dentry) != -1) // if the filename is found in the filesystem
    {
        int i;
        // go through fdt to find first file opening
        for (i = 2; i < 8; i++){ 
            if (global_pcb->fdt[i].flags == 0){ // once an open file space is found in the fdt
                // Populate file descriptor
                file_descriptor curr_fd;
                curr_fd.flags = 1; // Set file as open
                curr_fd.file_position = 0; // Initialize file position at start

                // Switch based on file type
                switch(dentry.file_type){
                    // RTC file type
                    case 0: {
                        curr_fd.fojtp = &rtc_ops;
                        curr_fd.inode = 0;
                    }
                    // Directory file type
                    case 1: {
                        curr_fd.fojtp = &directory_ops;
                        curr_fd.inode = 0;
                    }
                    // Regular data file type
                    case 2: {
                        curr_fd.fojtp = &data_file_ops;
                        curr_fd.inode = dentry.inode_num;
                    }
                    // Invalid file type
                    default:
                        return -1;
                    
                }
                // Populate PCD and call proper open
                global_pcb->fdt[i] = curr_fd;
                return (curr_fd.fojtp->open)(filename);
            }
        }
    }
    return -1;
}

int32_t do_close(int32_t fd)
{
    if ((fd >= 0 && fd < 8) && global_pcb->fdt[fd].flags == 1 && global_pcb->fdt[fd].fojtp->close != NULL)
    {
        // Check if pcb->flags = 0 (1 for opened 0 for closed), assign 0 if opened
        global_pcb->fdt[fd].flags = 0;
        return global_pcb->fdt[fd].fojtp->close(fd);
    }
    return -1;
}