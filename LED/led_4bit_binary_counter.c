/*
 * File    : led_4bit_binary_counter.c
 * Target  : LPC1768 (ARM Cortex-M3)
 * Purpose : Drives four LEDs on GPIO P1.21-P1.24 (upper half of Port 1) to
 *           display an incrementing binary count from 0 to 15 (0x0 to 0xF)
 *           in an infinite loop. Demonstrates GPIO port masking and
 *           upper-half byte register (FIOPINH) usage.
 *
 * Pin Map :
 *   P1.21 --> LED bit-0 (LSB)
 *   P1.22 --> LED bit-1
 *   P1.23 --> LED bit-2
 *   P1.24 --> LED bit-3 (MSB)
 */

#include <LPC17xx.h>

void delay(unsigned long int x);

int main()
{
    unsigned int count = 0;

    SystemInit();

    /* Power on GPIO1 (already on by default, but explicit is safe) */
    LPC_SC->PCONP |= (1 << 15);

    /* Configure P1.21-P1.24 as GPIO (clear function select bits) */
    LPC_PINCON->PINSEL3 &= ~(0xFFFF0000);

    /* Set P1.21-P1.24 (bits 5-8 of upper half) as OUTPUT */
    LPC_GPIO1->FIODIRH |= (0xF << 5);

    while(1)
    {
        /* Write counter value to LED pins (shifted into upper-byte position) */
        LPC_GPIO1->FIOPINH = (count << 5);
        delay(100000);

        /* Increment and wrap counter at 16 */
        count++;
        if(count >= 16) count = 0;
    }
}

/* Software delay loop */
void delay(unsigned long int count)
{
    unsigned long int i;
    for(i = 0; i < count; i++);
}
