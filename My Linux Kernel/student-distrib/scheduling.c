#include "scheduling.h"
#define EIGHT_MB 0x00800000
#define EIGHT_KB 0x00002000

// Lock ensures that no 2 processes attemp to switch context simultaneously, resulting in incorrect  PCBs. etc
// 0 if lock in use
int schedule_idx;
int tick_ = 0;

// LOCK FOR GLOBALS??

void schedule()
{
    cli();
    // disable_irq(0);

    // Store stack and frame pointer at point of interruption for when scheduler returns to this process

    // Check if process running in next round
    if (tick_ == 0)
    {
        tick_++;
        // page_tab[(VID_MEM_ADDR + (KB_4 * (0))) >> EXTRA_BITS] = (VID_MEM_ADDR + (KB_4 * (1))  | 0x7);
        // flush_tlb();
        // update_vid_mem(0, 1)
        running_shell = 0;
        sti();

        do_execute((uint8_t *)"shell0");
    }
    else if (tick_ == 1)
    {
        tick_++;

        page_tab[(VID_MEM_ADDR + (KB_4 * (1))) >> EXTRA_BITS] = ((VID_MEM_ADDR + (KB_4 * (1))) | 0x7);
        flush_tlb();

        memcpy((void *)(VID_MEM_ADDR + KB_4 * (1)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);

        asm volatile("\n\
                movl %%esp, %0 \n\
                movl %%ebp, %1"
                 : "=r"(global_pcb->curr_esp), "=r"(global_pcb->curr_ebp));
        global_procs[0] = *global_pcb;

        terminals[0].screen_x = get_screen_x();
        terminals[0].screen_y = get_screen_y();

        // memcpy((void *)(VID_MEM_ADDR + KB_4 * (1)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);
        // memcpy((void *)VID_MEM_ADDR, (void *)(VID_MEM_ADDR + KB_4 * (2)), (uint32_t)KB_4);
        // update_vidmem(2);
        // update_vid_mem(1, 0);

        // page_tab[(VID_MEM_ADDR + (KB_4 * (1))) >> EXTRA_BITS] = (VID_MEM_ADDR + (KB_4 * (1))  | 0x7);
        // flush_tlb();

        // memcpy((void *)(VID_MEM_ADDR + KB_4 * (1)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);
        memcpy((void *)VID_MEM_ADDR, (void *)(VID_MEM_ADDR + KB_4 * (2)), (uint32_t)KB_4);

        page_tab[(VID_MEM_ADDR + (KB_4 * (2))) >> EXTRA_BITS] = (VID_MEM_ADDR | 0x7);

        flush_tlb();
        set_screen_x(terminals[1].screen_x);
        set_screen_y(terminals[1].screen_y);
        update_vidmem(2);

        clear();
        /*
        // asm volatile("\n\
        // movl %%esp, %0 \n\
        // movl %%ebp, %1"
        //              : "=r"(global_procs[0].curr_esp), "=r"(global_procs[0].curr_ebp));
        */
        running_shell = 1;
        sti();
        do_execute((uint8_t *)"shell1");
    }
    else if (tick_ == 2)
    {
        tick_++;
        // memcpy((void *)(VID_MEM_ADDR + KB_4 * (2)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);
        // memcpy((void *)VID_MEM_ADDR, (void *)(VID_MEM_ADDR + KB_4 * (3)), (uint32_t)KB_4);
        // update_vidmem(3);
        // update_vid_mem(2, 1);
        // clear();

        page_tab[(VID_MEM_ADDR + (KB_4 * (2))) >> EXTRA_BITS] = ((VID_MEM_ADDR + (KB_4 * (2))) | 0x7);
        flush_tlb();

        memcpy((void *)(VID_MEM_ADDR + KB_4 * (2)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);

        asm volatile("\n\
                movl %%esp, %0 \n\
                movl %%ebp, %1"
                 : "=r"(global_pcb->curr_esp), "=r"(global_pcb->curr_ebp));
        global_procs[1] = *global_pcb;

        // page_tab[(VID_MEM_ADDR + (KB_4 * (2))) >> EXTRA_BITS] = (VID_MEM_ADDR + (KB_4 * (2))  | 0x7);
        // flush_tlb();

        // memcpy((void *)(VID_MEM_ADDR + KB_4 * (2)), (void *)VID_MEM_ADDR, (uint32_t)KB_4);
        terminals[0].screen_x = get_screen_x();
        terminals[0].screen_y = get_screen_y();
        memcpy((void *)VID_MEM_ADDR, (void *)(VID_MEM_ADDR + KB_4 * (3)), (uint32_t)KB_4);

        page_tab[(VID_MEM_ADDR + (KB_4 * (3))) >> EXTRA_BITS] = (VID_MEM_ADDR | 0x7);

        flush_tlb();

        update_vidmem(3);
        set_screen_x(terminals[2].screen_x);
        set_screen_y(terminals[2].screen_y);
        clear();
        running_shell = 2;
        sti();
        do_execute((uint8_t *)"shell2");

    } // If not NULLL switch
    else
    {
        
        // asm volatile("\n\
        // movl %%esp, %0 \n\
        // movl %%ebp, %1"
        //              : "=r"(global_pcb->curr_esp), "=r"(global_pcb->curr_ebp));

        // // Save current PCB and load new
        // global_procs[running_shell] = *global_pcb;

        asm volatile("\n\
                movl %%esp, %0 \n\
                movl %%ebp, %1"
                 : "=r"(global_pcb->curr_esp), "=r"(global_pcb->curr_ebp));
        global_procs[running_shell] = *global_pcb;



        // // Update running proc and schedule index
        //  // % 3 for 3 processes
        // // if(!global_procs[(schedule_idx + 1) % 3].curr_esp || !global_procs[(schedule_idx + 1) % 3].curr_ebp){
        // //     schedule_idx = (schedule_idx + 1) % 3;
        // //     return;
        // // }
        

        terminals[running_shell].screen_x = get_screen_x();
        terminals[running_shell].screen_y = get_screen_y();
        // tss.ss0 = KERNEL_DS;
       
        schedule_idx = (schedule_idx + 1) % 3;
        running_shell = schedule_idx;
        tss.ss0 = KERNEL_DS;
        tss.esp0  = EIGHT_MB - (( global_procs[running_shell].process_num+ 1) * EIGHT_KB) - 4;
        uint32_t physical_address = 0x800000 + (global_procs[running_shell].process_num * 0x400000); // 8 MB physical + offset 4 MB * proc_num
        page_dir[32] = (physical_address | 0x87); //01010111
        flush_tlb();

        update_vidmem(running_shell+1);
        set_screen_x(terminals[running_shell].screen_x);
        set_screen_y(terminals[running_shell].screen_y);
        // update_cursor();
        global_pcb = &global_procs[running_shell];
        running_proc = global_procs[running_shell].process_num;
    // Load new proc esp and ebp
        asm volatile("\n\
            movl %0, %%esp \n\
            movl %1, %%ebp"
                 :
                 : "r"(global_procs[running_shell].curr_esp), "r"(global_procs[running_shell].curr_ebp));
        // // Change TSS
        // tss.ss0 = KERNEL_DS;
        // tss.esp0 = 0x00800000 - ((global_procs[running_proc].process_num + 1) * 0x00002000) - 4; // 8 MB - num_processes * 2KB per process

        // // Update virtual mem
        // // physical memory offset for paging
        // uint32_t physical_address = 0x800000 + (global_procs[running_proc].process_num * 0x400000); // 8 MB physical + offset 4 MB * proc_num

        // // Refresh page dir
        // page_dir[32] = physical_address | 0x87; //01010111 // not poggers
        // flush_tlb();

        // // if (displayed_terminal == running_proc)
        // // {
        // //     update_vidmem(running_proc + 1, 1);
        // // }
        // // else
        // // {
        // //     update_vidmem(1 + running_proc, 0);
        // // }

        // global_pcb = &global_procs[running_proc];
        // // Load new proc esp and ebp
        
        //     asm volatile("\n\
        //     movl %0, %%esp \n\
        //     movl %1, %%ebp"
        //                 :
        //                 : "r"(global_procs[running_proc].curr_esp), "r"(global_procs[running_proc].curr_ebp));
        

        // // Update global PCB to new running process
        // // Wait until end to avoid synchronization issues
        
        // // sti();
        // enable_irq(0);
        // asm volatile("leave;"
        //              "ret;");
        
    }
    sti();
    // If currently unable to switch state simply continue executing this process if lock in use
    return;
}
