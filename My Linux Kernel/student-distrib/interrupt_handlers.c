#include "interrupt_handlers.h"
#include "i8259.h"
#include "lib.h"
#include "rtc.h"
#include "pit.h"
#include "terminal.h"

/*
    Function: RTC_handler
    Description: function called when pic raised rtc interrupt
    Inputs: none
    Outpouts: none
    side_effects: sets rtc clock
*/
void RTC_handler()
{
    asm volatile("pushal;");
    handle_rtc();
    asm volatile("popal;");
    asm("leave;"
        "iret;");
}

/*
    Function: keyboard_handler
    Description: function called when pic raised keyboard interrupt
    Inputs: none
    Outpouts: none
    side_effects: takes input data from the ps/2 port and handles it
                for now prints scancode letter to screen
*/
void keyboard_handler()
{

    asm volatile("pushal;");
    handle_input();
    asm volatile("popal;");
    asm("leave;"
        "iret;");
}

/*
    Function: PIT handler
    Description: function called when pit raises interrupt
    Inputs: none
    Outpouts: none
    side_effects: Scheduler switches process
*/
void PIT_handler(){
    asm volatile("pushal;");
    handle_pit();
    asm volatile("popal;");
    asm("leave;"
        "iret;");
}

