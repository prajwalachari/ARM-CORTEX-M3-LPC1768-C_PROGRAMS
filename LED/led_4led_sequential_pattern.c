/*
 * File    : led_4led_sequential_pattern.c
 * Target  : LPC1768 (ARM Cortex-M3)
 * Purpose : Cycles through a 4-LED sequential pattern on GPIO P1.24-P1.27.
 *           Each LED lights up one by one in a loop creating a chasing/
 *           running-light visual effect. Uses FIOCLR + FIOSET for clean
 *           pin transitions.
 *
 * Pin Map :
 *   P1.24 --> LED1 (Output)
 *   P1.25 --> LED2 (Output)
 *   P1.26 --> LED3 (Output)
 *   P1.27 --> LED4 (Output)
 */

#include <LPC17xx.h>

void delay(void);

int main(void)
{
    int i;
    SystemInit();

    /* Configure P1.24-P1.27 as GPIO (clear PINSEL3 function bits) */
    LPC_PINCON->PINSEL3 &= ~(0x0000FF00);

    /* Set P1.24-P1.27 as OUTPUT */
    LPC_GPIO1->FIODIR |= 0x0F000000;

    while(1)
    {
        /* Sequence 0 to 15: lights each LED individually (binary count) */
        for(i = 0; i < 16; i++)
        {
            LPC_GPIO1->FIOCLR = 0x0F000000;       /* Clear all 4 LEDs */
            LPC_GPIO1->FIOSET = (i << 24);        /* Light LEDs per binary value */
            delay();
        }
    }
}

/* Software delay using nested loop (~1 second) */
void delay(void)
{
    volatile int i, j;
    for(i = 0; i < 1000; i++)
    {
        for(j = 0; j < 1000; j++);
    }
}
