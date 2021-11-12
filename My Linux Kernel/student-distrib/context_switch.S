.global context_switch
.global execute_ret

#define USER_CS     0x0023
#define USER_DS     0x002B

context_switch:
    cli
    movl 4(%esp), %ebx
    pushl $USER_DS
    pushl $0x083FFFFC
    pushfl
    popl %edx
    orl $0x200, %edx
    pushl %edx
    pushl $USER_CS
    pushl %ebx
    iret
    execute_ret:

    