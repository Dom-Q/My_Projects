// all these handlers actually call the c code that prints
// the except and starts while loop

#ifndef _EXCEPTION_HANDLERS_H
#define _EXCEPTION_HANDLERS_H

#include "exceptions.h"

extern void division_by_0_handler(void);
extern void single_step_interrupt_handler(void);
extern void non_maskable_interrupt_handler(void);
extern void breakpoint_handler(void);
extern void overflow_handler(void);
extern void bound_range_exceeded_handler(void);
extern void invalid_opcode_handler(void);
extern void coprocessor_unavailable_handler(void);
extern void double_fault_handler(void);
extern void coprocessor_segment_overrun_handler(void);
extern void invalid_tss_handler(void);
extern void segment_not_present_handler(void);
extern void stack_segmentation_fault_handler(void);
extern void protection_fault_handler(void);
extern void page_fault_handler(void);
extern void reserved_handler(void);
extern void x87_floating_point_exception_handler(void);
extern void alignment_check_handler(void);
extern void machine_check_handler(void);
extern void simd_floating_point_exception_handler(void);
extern void virtualization_exception_handler(void);
extern void control_protection_exception_handler(void);

#endif
