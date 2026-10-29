/*
 * ============================================================================
 * File        : led_switch_button_p0_to_p1_v1.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0 & Port 1
 * Description : Digital input polling with LED output indicator.
 *               Reads logic level from input pin P0.0 (push button / switch).
 *               When P0.0 is HIGH, turns ON test LED on P1.29.
 *               When P0.0 is LOW, turns OFF test LED on P1.29.
 * Pin Mapping :
 *   P0.0  --> Push Button / Switch Input
 *   P1.29 --> On-board / External LED Output
 * ============================================================================
 */

#include <LPC17xx.h>

int main(void)
{
    SystemInit();
		LPC_SC->PCONP |= 0x00008000;
    LPC_GPIO0->FIODIR &= 0xFFFFFFFF;     // Clear bit 0
		
    LPC_GPIO1->FIODIR |= 0x20000000;     // Set bit 29 

    while(1)
    {
        if(LPC_GPIO0->FIOPIN & 0x00000001)   // Check bit 0
        {
            LPC_GPIO1->FIOSET = 0x20000000;  // (P1.29 = 1)
        }
        else
        {
            LPC_GPIO1->FIOCLR = 0x20000000;  // (P1.29 = 0)
        }
    }
}


















/*#include<LPc17xx.h>
void delay (unsigned long x);
int main()
{
	int val;
	SystemInit();
	LPC_GPIO0 -> FIOMASK0=0x00;
	LPC_GPIO0 -> FIODIR0=0x01;
	LPC_GPIO1 -> FIOMASK3=0xDF;
	LPC_GPIO1 -> FIODIR3=0x20;
	while(1)
	{
	  val=LPC_GPIO0 -> FIOPIN0;
	  if(val==0x01)
	  {
	    LPC_GPIO1 -> FIOSET3=0x20;
		}
		else
		{
		LPC_GPIO1 -> FIOCLR3=0x20;
		}
	}
}*/
