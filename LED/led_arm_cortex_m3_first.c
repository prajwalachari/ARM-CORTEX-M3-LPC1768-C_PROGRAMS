/*
 * File    : led_arm_cortex_m3_first.c
 * Target  : LPC1768 (ARM Cortex-M3)
 * Purpose : First LED blink program for the ARM Cortex-M3 LPC1768 board.
 *           Blinks 4 LEDs on P0.22-P0.25 simultaneously using FIOSET/FIOCLR.
 *           This is the introductory bare-metal GPIO exercise demonstrating
 *           basic clock initialization and GPIO output setup.
 *
 * Pin Map :
 *   P0.22 --> LED1
 *   P0.23 --> LED2
 *   P0.24 --> LED3
 *   P0.25 --> LED4
 */

#include <LPC17xx.h>

#define LED1    (1 << 22)
#define LED2    (1 << 23)
#define LED3    (1 << 24)
#define LED4    (1 << 25)
#define LED_ALL (LED1 | LED2 | LED3 | LED4)

void delay(void);

int main(void)
{
    SystemInit();

    /* Set P0.22-P0.25 as OUTPUT */
    LPC_GPIO0->FIODIR = LED_ALL;

    while(1)
    {
        /* Turn ON all LEDs */
        LPC_GPIO0->FIOSET = LED_ALL;
        delay();

        /* Turn OFF all LEDs */
        LPC_GPIO0->FIOCLR = LED_ALL;
        delay();
    }
}

/* Software busy-wait delay */
void delay(void)
{
    volatile int i;
    for(i = 0; i < 3000000; i++);
}
