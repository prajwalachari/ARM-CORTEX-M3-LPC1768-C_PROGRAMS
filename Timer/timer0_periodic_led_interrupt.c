/*
 * ============================================================================
 * File        : timer0_periodic_led_interrupt.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : Timer0 & GPIO Port 1
 * Description : Hardware Timer0 configuration with match interrupt.
 *               Programs Prescaler (PR) and Match Register (MR0) with interrupt
 *               and reset on match (MCR = 3), enabling periodic interrupt-driven
 *               timing while the main loop drives LED pin P1.29.
 * Pin Mapping :
 *   P1.29 --> Periodic Indicator LED (Output)
 * ============================================================================
 */

#include<stdio.h>
unsigned long int delay();



void TIMER0_IRQHandler(void)
{
	
}
int main()
{
	
	SystemInit();
	initTimer();
	LPC_GPIO1 -> FIOMASK3 = 0XDF;//1101 1111
	LPC_GPIO1 -> FIODIR3 = 0X20;
	
	LPC_SC -> PCONP |= 0X00000002;
	LPC_TIM0 -> TCR = 0X00000002;
	LPC_TIM0 -> PR = 0X00000018 ;
	LPC_TIM0 -> MR0 = 0X000003E7;
	LPC_TIM0 -> MCR = 0X00000003;
	NVIC_EnableIRQ(TIMER0_IRQn);
	LPC_TIM0 -> TCR = 0X00000001 ;
	while(1)
	{
		LPC_GPIO1 -> FIOSET3 = 0X20;
		delay(0x1000);
		LPC_GPIO1 -> FIOCLR3 = 0X20;
		delay(0x1000);
	
	}
	

	
}

unsigned long int delay(void)
{
	int i = 0;
	for(i=0;i<3000;i++)
}
