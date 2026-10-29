/*
 * File    : led_blink_4leds_simultaneous.c
 * Target  : LPC1768 (ARM Cortex-M3)
 * Purpose : Blinks four external LEDs connected to GPIO P0.22, P0.23, P0.24,
 *           and P0.25 simultaneously (all ON then all OFF).
 *           Demonstrates GPIO group output using FIOPIN register assignment.
 *
 * Pin Map :
 *   P0.22 --> LED1 (Output)
 *   P0.23 --> LED2 (Output)
 *   P0.24 --> LED3 (Output)
 *   P0.25 --> LED4 (Output)
 */

#include <LPC17xx.h>

#define LED1     0x00400000   /* P0.22 */
#define LED2     0x00800000   /* P0.23 */
#define LED3     0x01000000   /* P0.24 */
#define LED4     0x02000000   /* P0.25 */

#define LED_ALL  0x03C00000   /* All four LEDs P0.22-P0.25 */

void delay(unsigned int count);

int main(void)
{
    SystemInit();

    /* Set P0.22-P0.25 as OUTPUT */
    LPC_GPIO0->FIODIR = LED_ALL;

    while(1)
    {
        /* Turn ON all four LEDs */
        LPC_GPIO0->FIOPIN = LED_ALL;
        delay(1000000);

        /* Turn OFF all four LEDs */
        LPC_GPIO0->FIOPIN = 0x00000000;
        delay(1000000);
    }
}

/* Software delay loop */
void delay(unsigned int count)
{
    volatile unsigned int i;
    for(i = 0; i < count; i++);
}
