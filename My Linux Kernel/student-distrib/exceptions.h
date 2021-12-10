#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include "lib.h"
#include "system_calls.h"

/* EXCEPTION FUNCTIONS */
void division_by_0();
void single_step_interrupt();
void non_maskable_interrupt();
void breakpoint();
void overflow();
void bound_range_exceeded();
void invalid_opcode();
void coprocessor_unavailable();
void double_fault();
void coprocessor_segment_overrun();
void invalid_tss();
void segment_not_present();
void stack_segmentation_fault();
void protection_fault();
void page_fault();
void reserved();
void x87_floating_point_exception();
void alignment_check();
void machine_check();
void simd_floating_point_exception();
void virtualization_exception();
void control_protection_exception();

#endif

