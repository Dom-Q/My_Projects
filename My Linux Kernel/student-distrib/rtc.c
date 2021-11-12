#include "lib.h"
#include "rtc.h"
#include "i8259.h"

#include "types.h"
#include "x86_desc.h"
#include "paging.h"

// set rtc and initialize values
volatile int flag;

void init_rtc()
{
    enable_irq(2); // enable port on master for slave pic
    enable_irq(8); // enable port 0 of slave pic
    outb(0x8B, 0x70);
    char prev = inb(0x71);
    outb(0x8B, 0x70);
    outb(prev | 0x40, 0x71);
}

// actually handle interrupts
void handle_rtc()
{
    cli();
    //test_interrupts();
    //printf(".");
    outb(0x0C, 0x70); // select register C
    inb(0x71);        // just throw away contents
    flag = 1;
    send_eoi(8);
    sti();
}

/* 
 * rtc_open
 *  DESCRIPTION:    initializes RTC frequency to 2Hz, return 0
 *  INPUTS:         none           
 *  OUTPUTS:        none
 *  RETURN VALUE:   0
 *  SIDE EFFECTS:   switched RTC interrupt frequency to 2Hz
 */
int32_t rtc_open(const uint8_t *filename)
{
    int16_t frequency = 2; //2Hz
    int16_t buf[] = {frequency};
    rtc_write(0, buf, 128);
    return 0;
}

/* 
 * rtc_read
 *  DESCRIPTION:    block until the next interrupt, return 0
 *  INPUTS:         none
 *  OUTPUTS:        none
 *  RETURN VALUE:   0 on a new RTC cycle
 *  SIDE EFFECTS:   cycles until RTC interrupt
 */
int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes)
{
    flag = 0;
    while (!flag)
    {
    }
    return 0;
}

/* 
 * rtc_write
 *  DESCRIPTION:    change frequency, return 0 or -1      
 *                  - Frequencies must be power of 2 
 *  INPUTS:         freq - New frequency
 *  OUTPUTS:        none
 *  RETURN VALUE:   0 on success, -1 on failure (invalid input)
 *  SIDE EFFECTS:   Changes RTC interrupt frequency to desired between (2, 1024)
 */
int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes)
{
    uint16_t* freq = (uint16_t*)buf;

    //check if frequency is a power of 2
    if (!(freq[0] && (!(freq[0] & (freq[0] - 1)))))
    {
        return -1;
    }

    int rate; //frequency =  32768 >> (rate-1)
    int i;
    //calculate the rate to write to RTC Register A
    for (i = 0; i < 15; i++)
    {
        if (freq[0] == 1 << i)
        {
            rate = 16 - i;
        }
    }

    //verify that frequency<=1024Hz and frequency>1
    if ((rate < 6) | (rate > 15))
    {
        return -1;
    }

    //write new frequency to RTC Register A
    cli();
    outb(0x8A, 0x70);                   //set index to register A, disable NMI
    char prev = inb(0x71);              //get initial value of register A
    outb(0x8A, 0x70);                   //reset index to A
    outb(((prev & 0xF0) | rate), 0x71); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();

    return 0;
}

/* 
 * rtc_close
 *  DESCRIPTION:    probably does nothing, unless you virtualize RTC, return 0
 *  INPUTS:         none
 *  OUTPUTS:        none
 *  RETURN VALUE:   0
 *  SIDE EFFECTS:   none
 */
int32_t rtc_close(int32_t fd)
{
    return 0;
}


