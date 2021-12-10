#include "idt_init.h"

/*
    Function: init_idt
    inputs: none
    outputs: none
    side effects: runs through all members of the IDT and initializes their values based
    on if they are exeptions or inturupts then fills the values for the handler functions 
*/
void init_idt()
{
    int i;
    //run through all exeptions to initialize them
    for (i = 0; i < 32; i++)
    {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].size = 1;
        idt[i].dpl = 0;
        idt[i].present = 1;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1; // must be set to one for exeptions
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        //dont need to change either offset
    }
    //run through all inturupts to initialize them
    for (i = 32; i < 256; i++)
    {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].size = 1;
        idt[i].dpl = 0;
        idt[i].present = 1;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0; // must be set to zero for functions
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        //dont need to change either offset
    }
    idt[128].dpl = 3;
    idt[128].reserved3 = 1;

    //Set exeption handlers for exeptions
    SET_IDT_ENTRY(idt[0], division_by_0_handler);
    SET_IDT_ENTRY(idt[1], single_step_interrupt_handler);
    SET_IDT_ENTRY(idt[2], non_maskable_interrupt_handler);
    SET_IDT_ENTRY(idt[3], breakpoint_handler);
    SET_IDT_ENTRY(idt[4], overflow_handler);
    SET_IDT_ENTRY(idt[5], bound_range_exceeded_handler);
    SET_IDT_ENTRY(idt[6], invalid_opcode_handler);
    SET_IDT_ENTRY(idt[7], coprocessor_unavailable_handler);
    SET_IDT_ENTRY(idt[8], double_fault_handler);
    SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun_handler);
    SET_IDT_ENTRY(idt[10], invalid_tss_handler);
    SET_IDT_ENTRY(idt[11], segment_not_present_handler);
    SET_IDT_ENTRY(idt[12], stack_segmentation_fault_handler);
    SET_IDT_ENTRY(idt[13], protection_fault_handler);
    SET_IDT_ENTRY(idt[14], page_fault_handler);
    SET_IDT_ENTRY(idt[15], reserved_handler);
    SET_IDT_ENTRY(idt[16], x87_floating_point_exception_handler);
    SET_IDT_ENTRY(idt[17], alignment_check_handler);
    SET_IDT_ENTRY(idt[18], machine_check_handler);
    SET_IDT_ENTRY(idt[19], simd_floating_point_exception_handler);
    SET_IDT_ENTRY(idt[20], virtualization_exception_handler);
    SET_IDT_ENTRY(idt[21], control_protection_exception_handler);
    //fill in interupt handler offset for all intrupts currently handled
    SET_IDT_ENTRY(idt[40], RTC_handler);
    SET_IDT_ENTRY(idt[33], keyboard_handler);
    SET_IDT_ENTRY(idt[32], PIT_handler);

    // System Calls
    // Soft interrupts at 0x80
    SET_IDT_ENTRY(idt[128], syscall_handler);
}
