#include "pit.h"

#define MODE_3 0x36
#define PIT_MODE_PORT 0x43
#define PIT_FREQ 5120
#define CH_0 0x40

int tick = 0;
/*
 * init_pit
 *  DESCRIPTION:    initializes Programmable Interval Timer. Primary use is scheduling.
 *  INPUTS:         none
 *  OUTPUTS:        none
 *  RETURN VALUE:   void
 *  SIDE EFFECTS:    with khz frequency 
 */

void init_pit()
{
    //Sets MODE_3(square wave generator mode) on
    outb(MODE_3, PIT_MODE_PORT);
    //write to the byte0
    outb(PIT_FREQ & 0xFF, CH_0);
    //write to the byte1
    outb(PIT_FREQ >> 8, CH_0);
    // running_shell = 0;
    enable_irq(0);
}

void handle_pit()
{
    send_eoi(0);
    schedule();
    tick = 1;
}

//testing
//waits for a tick from
void get_tick()
{
    tick = 0;
    while (!tick)
    {
    }
}
