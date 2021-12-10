.global context_switch
.global execute_ret

#define USER_CS     0x0023
#define USER_DS     0x002B

/* 
*  Function: context_switch
*  Input: New context EIP
*  Output: None
*  Return value: None 
*  Side effects: Change state to new context
*/
context_switch:
    cli
    movl 4(%esp), %ebx # Store EIP in ebx
    addl $8, %esp # prepare stack pointer
    pushl $USER_DS # Set privilege level
    # Set flags

    # movl %EFLAGS, %edx
    # Push flags onto stack and retrieve since eflags not accessible with movl
    pushl $0x083FFFFC 
    pushfl
    popl %edx

    orl $512, %edx # 001000000000 - set interrupt flag 
    
    # Push EIP and 
    pushl %edx
    pushl $USER_CS
    pushl %ebx
    # Iret to jump new context
    iret
