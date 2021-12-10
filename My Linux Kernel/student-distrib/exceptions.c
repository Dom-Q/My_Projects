/* EXCEPTION FUNCTIONS */
/*
    This File contains the Exeption functions for all exeptions the proccessor might send
    Each function is void and prints the exception that raised it
    They all then put the system in an infinite while loop instead of actually handling the exeption fornow
    TODO: add handler code to each to do the correct work for each exception.
*/
#include "exceptions.h"

const uint8_t exception_num = 0;
int exception = 0;

void division_by_0()
{
    printf("Exception: Divide by 0");
    exception = 1;
    do_halt(exception_num);
}

void single_step_interrupt()
{
    printf("Exception: Single Step Interrupt\n");
    exception = 1;
    do_halt(exception_num);
}

void non_maskable_interrupt()
{
    printf("Exception: NMI\n");
    exception = 1;
    do_halt(exception_num);
}

void breakpoint()
{
    printf("Exception: Breakpoint\n");
    exception = 1;
    do_halt(exception_num);
}

void overflow()
{
    printf("Exception: Overflow\n");
    exception = 1;
    do_halt(exception_num);
}

void bound_range_exceeded()
{
    printf("Exception: Bound Range Exceeded\n");
    exception = 1;
    do_halt(exception_num);
}

void invalid_opcode()
{
    printf("Exception: Invalid Operation\n");
    exception = 1;
    do_halt(exception_num);
}

void coprocessor_unavailable()
{
    printf("Exception: Coprocessor Not Available\n");
    exception = 1;
    do_halt(exception_num);
}

void double_fault()
{
    printf("Exception: Double Fault\n");
    exception = 1;
    do_halt(exception_num);
}

void coprocessor_segment_overrun()
{
    printf("Exception: Coprocessor Segment Overrun\n");
    exception = 1;
    do_halt(exception_num);
}

void invalid_tss()
{
    printf("Exception: Invalid Task State Segment\n");
    exception = 1;
    do_halt(exception_num);
}

void segment_not_present()
{
    printf("Exception: Segment not Present\n");
    exception = 1;
    do_halt(exception_num);
}

void stack_segmentation_fault()
{
    printf("Exception: Stack Segment Fault\n");
    exception = 1;
    do_halt(exception_num);
}

void protection_fault()
{
    printf("Exception: Protection Fault\n");
    exception = 1;
    do_halt(exception_num);
}

void page_fault()
{
    printf("Exception: Page Fault\n");
    exception = 1;
    do_halt(exception_num);
}

void reserved()
{
    printf("Exception: Reserved\n");
    exception = 1;
    do_halt(exception_num);
}

void x87_floating_point_exception()
{
    printf("Exception: x87 Floating Point Error\n");
    exception = 1;
    do_halt(exception_num);
}

void alignment_check()
{
    printf("Exception: Alignment Check\n");
    exception = 1;
    do_halt(exception_num);
}

void machine_check()
{
    printf("Exception: Machine Check\n");
    exception = 1;
    do_halt(exception_num);
}

void simd_floating_point_exception()
{
    printf("Exception: SIMD Floating Point Excetption\n");
    exception = 1;
    do_halt(exception_num);
}

void virtualization_exception()
{
    printf("Exception: Virtualization Error\n");
    exception = 1;
    do_halt(exception_num);
}

void control_protection_exception()
{
    printf("Exception: Control Protection Error\n");
    exception = 1;
    do_halt(exception_num);
}
