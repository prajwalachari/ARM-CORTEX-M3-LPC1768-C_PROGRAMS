/*
 * File    : led_onboard_input_controlled.c
 * Target  : LPC1768 (ARM Cortex-M3)
 * Purpose : Reads a digital input button on P0.0 and mirrors its state
 *           to the on-board LED on P1.29.
 *           When button is HIGH  -> LED turns ON
 *           When button is LOW   -> LED turns OFF
 *           Demonstrates basic GPIO input polling and output driving.
 *
 * Pin Map :
 *   P0.0  --> Push Button / Digital Input (Input)
 *   P1.29 --> On-board LED (Output)
 */

#include <LPC17xx.h>

int main(void)
{
    SystemInit();

    /* P1.29 as OUTPUT (on-board LED) */
    LPC_GPIO1->FIOMASK &= ~(1 << 29);
    LPC_GPIO1->FIODIR  |=  (1 << 29);

    /* P0.0 as INPUT (push button) */
    LPC_GPIO0->FIOMASK &= ~(1 << 0);
    LPC_GPIO0->FIODIR  &= ~(1 << 0);

    while(1)
    {
        /* Mirror button state to LED */
        if(LPC_GPIO0->FIOPIN & (1 << 0))
        {
            LPC_GPIO1->FIOSET = (1 << 29); /* Button HIGH -> LED ON */
        }
        else
        {
            LPC_GPIO1->FIOCLR = (1 << 29); /* Button LOW  -> LED OFF */
        }
    }
}
