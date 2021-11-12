

.global division_by_0_handler, single_step_interrupt_handler, non_maskable_interrupt_handler
.global breakpoint_handler, overflow_handler, bound_range_exceeded_handler
.global invalid_opcode_handler, coprocessor_unavailable_handler, double_fault_handler
.global coprocessor_segment_overrun_handler, invalid_tss_handler, segment_not_present_handler
.global stack_segmentation_fault_handler, protection_fault_handler, page_fault_handler
.global reserved_handler, x87_floating_point_exception_handler, alignment_check_handler
.global machine_check_handler, simd_floating_point_exception_handler, virtualization_exception_handler, control_protection_exception_handler

# assembly code that pushes all registers and flags to stack, then pops them
# after calling the appropriate c function

division_by_0_handler:
    cli
	pushal 
	pushfl   
	call division_by_0
	popfl    
	popal    
	iret
	sti

single_step_interrupt_handler:
	cli
	pushal    
	pushfl   
	call single_step_interrupt
	popfl    
	popal    
	iret
	sti

non_maskable_interrupt_handler:
	cli
	pushal    
	pushfl   
	call non_maskable_interrupt
	popfl    
	popal    
	iret
	sti

breakpoint_handler:
	cli
	pushal    
	pushfl   
	call breakpoint
	popfl    
	popal    
	iret
	sti

overflow_handler:
	cli
	pushal    
	pushfl   
	call overflow
	popfl    
	popal    
	iret
	sti

bound_range_exceeded_handler:
	cli
	pushal    
	pushfl   
	call bound_range_exceeded
	popfl    
	popal    
	iret
	sti

invalid_opcode_handler:
	cli
	pushal    
	pushfl   
	call invalid_opcode
	popfl    
	popal    
	iret
	sti

coprocessor_unavailable_handler:
	cli
	pushal    
	pushfl   
	call coprocessor_unavailable
	popfl    
	popal    
	iret
	sti

double_fault_handler:
	cli
	pushal    
	pushfl   
	call double_fault
	popfl    
	popal    
	iret
	sti

coprocessor_segment_overrun_handler:
	cli
	pushal    
	pushfl   
	call coprocessor_segment_overrun
	popfl    
	popal    
	iret
	sti

invalid_tss_handler:
	cli
	pushal    
	pushfl   
	call invalid_tss
	popfl    
	popal    
	iret
	sti

segment_not_present_handler:
	cli
	pushal    
	pushfl   
	call segment_not_present
	popfl    
	popal    
	iret
	sti

stack_segmentation_fault_handler:
	cli
	pushal    
	pushfl   
	call stack_segmentation_fault
	popfl    
	popal    
	iret
	sti

protection_fault_handler:
	cli
	pushal    
	pushfl   
	call protection_fault
	popfl    
	popal    
	iret
	sti

page_fault_handler:
	cli
	pushal    
	pushfl   
	call page_fault
	popfl    
	popal    
	iret
	sti

reserved_handler:
	cli
	pushal    
	pushfl   
	call reserved
	popfl    
	popal    
	iret
	sti

x87_floating_point_exception_handler:
	cli
	pushal    
	pushfl   
	call x87_floating_point_exception
	popfl    
	popal    
	iret
	sti

alignment_check_handler:
	cli
	pushal    
	pushfl   
	call alignment_check
	popfl    
	popal    
	iret
	sti

machine_check_handler:
	cli
	pushal    
	pushfl   
	call machine_check
	popfl    
	popal    
	iret
	sti

simd_floating_point_exception_handler:
	cli
	pushal    
	pushfl   
	call simd_floating_point_exception
	popfl    
	popal    
	iret
	sti

virtualization_exception_handler:
	cli
	pushal    
	pushfl   
	call virtualization_exception
	popfl    
	popal    
	iret
	sti

control_protection_exception_handler:
	cli
	pushal    
	pushfl   
	call control_protection_exception
	popfl    
	popal    
	iret
	sti



