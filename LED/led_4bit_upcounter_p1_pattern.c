/*
 * ============================================================================
 * File        : led_4bit_upcounter_p1_pattern.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 1 (P1.24 - P1.27)
 * Description : 4-bit binary up-counter pattern on GPIO pins P1.24 through P1.27.
 *               Sequentially counts from 0 to 15 (0x0 to 0xF) in a continuous
 *               loop with software delay, visually displaying binary states.
 * Pin Mapping :
 *   P1.24 --> LED bit-0 (LSB)
 *   P1.25 --> LED bit-1
 *   P1.26 --> LED bit-2
 *   P1.27 --> LED bit-3 (MSB)
 * ============================================================================
 */

//up counter

#include <LPC17xx.h>

void delay(void);

int main(void){
    int i;
    SystemInit();

    LPC_PINCON->PINSEL3 &= ~(0x0000FF00);
    LPC_GPIO1->FIODIR |= 0x0F000000;

    while(1)
    {
        for(i=0;i<16;i++)//use i from 15to 0 for down counter but rest rest same 
        {
					LPC_GPIO1->FIOCLR = 0x0F000000;
					LPC_GPIO1->FIOSET = (i << 24);
					delay(0x50000);
        }
    }
}
