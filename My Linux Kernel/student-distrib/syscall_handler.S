
.global syscall_handler

first_call: .long 0x00000001 # last call val
last_call: .long 0x00001010 # first call val
ret_val: .long 0x0

# System call jump table
JUMPTABLE:
    .long do_halt, do_execute, do_read, do_write, do_open, do_close

# Dispather
# eax - call number
# ebx - arg1
# ecx - arg2
# edx - arg3
syscall_handler:
    pushfl
    pushal

    # check command validity
    cmpl first_call, %eax
    jl INVALID_CMD
    cmpl last_call, %eax
    jg INVALID_CMD
    subl $1, %eax
    # Push arguments right->left
    pushl %edx
    pushl %ecx
    pushl %ebx

    # Call system call using jump table
    call *JUMPTABLE(, %eax, 4)

    # Save ret val
    movl %eax, ret_val

    # Remove args
    addl $12, %esp
    jmp FIN

INVALID_CMD:
    movl $-1, ret_val
    # Restore caller regs
FIN:
    popal
    popfl

    # Restore ret val into eax
    movl ret_val, %eax
    
    leave
    ret

