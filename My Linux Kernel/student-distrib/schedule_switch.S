.global schedule_switch


#define USER_CS     0x0023
#define USER_DS     0x002B

/* 
*  Function: context_switch
*  Input: New context EIP
*  Output: None
*  Return value: None 
*  Side effects: Change state to new context
*/
schedule_switch:
    leave
    # Iret to jump new context
    iret
