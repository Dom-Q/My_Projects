/* EXCEPTION FUNCTIONS */
/*
    This File contains the Exeption functions for all exeptions the proccessor might send
    Each function is void and prints the exception that raised it
    They all then put the system in an infinite while loop instead of actually handling the exeption fornow
    TODO: add handler code to each to do the correct work for each exception.
*/

#include "lib.h"
#include "exceptions.h"
void division_by_0()
{
    printf("Exception: Divide by 0");
    while (1)
    {
    }
}

void single_step_interrupt()
{
    printf("Exception: Single Step Interrupt\n");
    while (1)
    {
    }
}

void non_maskable_interrupt()
{
    printf("Exception: NMI\n");
    while (1)
    {
    }
}

void breakpoint()
{
    printf("Exception: Breakpoint\n");
    while (1)
    {
    }
}

void overflow()
{
    printf("Exception: Overflow\n");
    while (1)
    {
    }
}

void bound_range_exceeded()
{
    printf("Exception: Bound Range Exceeded\n");
    while (1)
    {
    }
}

void invalid_opcode()
{
    printf("Exception: Invalid Operation\n");
    while (1)
    {
    }
}

void coprocessor_unavailable()
{
    printf("Exception: Coprocessor Not Available\n");
    while (1)
    {
    }
}

void double_fault()
{
    printf("Exception: Double Fault\n");
    while (1)
    {
    }
}

void coprocessor_segment_overrun()
{
    printf("Exception: Coprocessor Segment Overrun\n");
    while (1)
    {
    }
}

void invalid_tss()
{
    printf("Exception: Invalid Task State Segment\n");
    while (1)
    {
    }
}

void segment_not_present()
{
    printf("Exception: Segment not Present\n");
    while (1)
    {
    }
}

void stack_segmentation_fault()
{
    printf("Exception: Stack Segment Fault\n");
    while (1)
    {
    }
}

void protection_fault()
{
    printf("Exception: Protection Fault\n");
    while (1)
    {
    }
}

void page_fault()
{
    printf("Exception: Page Fault\n");
    while (1)
    {
    }
}

void reserved()
{
    printf("Exception: Reserved\n");
    while (1)
    {
    }
}

void x87_floating_point_exception()
{
    printf("Exception: x87 Floating Point Error\n");
    while (1)
    {
    }
}

void alignment_check()
{
    printf("Exception: Alignment Check\n");
    while (1)
    {
    }
}

void machine_check()
{
    printf("Exception: Machine Check\n");
    while (1)
    {
    }
}

void simd_floating_point_exception()
{
    printf("Exception: SIMD Floating Point Excetption\n");
    while (1)
    {
    }
}

void virtualization_exception()
{
    printf("Exception: Virtualization Error\n");
    while (1)
    {
    }
}

void control_protection_exception()
{
    printf("Exception: Control Protection Error\n");
    while (1)
    {
    }
}
