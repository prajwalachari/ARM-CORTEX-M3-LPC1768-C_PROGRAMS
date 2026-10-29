/*
 * File    : led_8bit_binary_counter.c
 * Target  : LPC1768 (ARM Cortex-M3)
 * Purpose : Drives 8 external LEDs on GPIO P1.22-P1.29 to display an
 *           8-bit binary up-counter (0 to 255) with a visible delay between
 *           each step. Demonstrates use of FIOSETH / FIOSETL byte registers
 *           for clean multi-LED updates.
 *
 * Pin Map :
 *   P1.22 --> LED bit-0 (LSB)
 *   ...
 *   P1.29 --> LED bit-7 (MSB)
 */

#include <LPC17xx.h>

void delay(int x);

int main()
{
    unsigned int count = 0;

    SystemInit();

    /* Set P1.22-P1.29 (upper half) as OUTPUT */
    LPC_GPIO1->FIODIRH |= 0x1FE0; /* bits 5-12 of upper half = P1.22-P1.29 */

    while(1)
    {
        /* Clear LED pins then write current count */
        LPC_GPIO1->FIOSETH = (count << 6);

        delay(0x2000000);

        /* Clear before next update */
        LPC_GPIO1->FIOCLRH = 0x1FE0;

        count++;

        /* Wrap at 256 (8-bit rollover) */
        if(count == 256)
        {
            count = 0;
        }
    }
}

/* Software delay loop */
void delay(int x)
{
    volatile unsigned int i;
    for(i = 0; i < x; i++);
}
