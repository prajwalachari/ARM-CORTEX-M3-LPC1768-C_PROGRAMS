/*
 * File    : led_blink_single.c
 * Target  : LPC1768 (ARM Cortex-M3)
 * Purpose : Blinks the on-board LED connected to GPIO P1.29 continuously.
 *           Uses a software delay loop to create a visible toggle period.
 *           Demonstrates basic GPIO output configuration using FIODIR,
 *           FIOSET, and FIOCLR registers.
 *
 * Pin Map :
 *   P1.29 --> On-board LED (Output)
 */

#include <LPC17xx.h>

void delay(long int x);

int main(void)
{
    SystemInit();

    /* Mask all bits except P1.29; set P1.29 as OUTPUT */
    LPC_GPIO1->FIOMASK = 0xDFFFFFFF;
    LPC_GPIO1->FIODIR |= 0x20000000;

    while(1)
    {
        /* Turn LED ON */
        LPC_GPIO1->FIOSET = 0x20000000;
        delay(0x5000);

        /* Turn LED OFF */
        LPC_GPIO1->FIOCLR = 0x20000000;
        delay(0x5000);
    }
}

/* Software delay loop */
void delay(long int x)
{
    unsigned long int i;
    for(i = 0; i < x; i++)
    {}
}
