/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void)
{
    master_mask = 0xFD;
    slave_mask = 0xFF;

    // Send initializiation code to both PICs
    // wati in between to account for slow speed of device

    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    // Send offset in vector table

    outb(ICW2_MASTER, MASTER_DATA_PORT);
    outb(ICW2_SLAVE, SLAVE_DATA_PORT); // Not x20-x28?
    // Notify master/slave status

    outb(ICW3_MASTER, MASTER_DATA_PORT); // Notify master of slave connected
    outb(ICW3_SLAVE, SLAVE_DATA_PORT);   // Notify slave of cascade

    // Send 3rd command with additional information

    outb(ICW4, MASTER_DATA_PORT);
    outb(ICW4, SLAVE_DATA_PORT);

    //set bitmasks for the irqs to disable all irqs but the slave pic
    outb(master_mask, MASTER_DATA_PORT);
    outb(slave_mask, SLAVE_DATA_PORT);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num)
{
    if (irq_num > 15)
        return; // Error Check

    uint8_t port;
    // Check if IRQ num member of slave port
    if (irq_num >= NUM_PORTS)
    {
        enable_irq(2);
        irq_num -= NUM_PORTS;
        slave_mask = (slave_mask & ~(1 << irq_num));
        outb(slave_mask, SLAVE_DATA_PORT);

    }
    else
    {
        port = MASTER_DATA_PORT;
        master_mask = (master_mask & ~(1 << irq_num));
        outb(master_mask, MASTER_DATA_PORT);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num)
{
    if (irq_num > 15)
        return; // Error Check

    uint8_t port;
    // Check if IRQ num member of slave port
    if (irq_num >= NUM_PORTS)
    {
        irq_num -= NUM_PORTS;
        slave_mask = (slave_mask | (1 << irq_num));
        outb(slave_mask, SLAVE_DATA_PORT);
    }
    else
    {
        port = MASTER_DATA_PORT;
        master_mask = (master_mask | (1 << irq_num));
        outb(master_mask, MASTER_DATA_PORT);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num)
{
    if (irq_num >= NUM_PORTS) {
        outb(EOI | (irq_num - NUM_PORTS), SLAVE_8259_PORT);
        send_eoi(2);
    }

    outb((EOI | irq_num), MASTER_8259_PORT);
}
