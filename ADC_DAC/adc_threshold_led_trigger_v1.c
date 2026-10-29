/*
 * ============================================================================
 * File        : adc_threshold_led_trigger_v1.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : ADC (AD0.5) & GPIO Port 1
 * Description : Analog threshold comparator using polling on ADSTAT.
 *               Reads potentiometer/sensor voltage on AD0.5 (P1.31).
 *               If reading > 0x4D9 (~1.0V with 3.3V Vref), turns ON LED P1.29.
 *               Otherwise turns OFF LED P1.29.
 * Pin Mapping :
 *   P1.31 --> AD0.5 (Analog Input)
 *   P1.29 --> Threshold Indicator LED (Output)
 * ============================================================================
 */

#include <LPC17xx.h>
unsigned short int adc=0;
int main()
{
	SystemInit();
	LPC_SC->PCONP |= 0x00001000;
	LPC_PINCON->PINSEL3 |= 0xC0000000;  //P1.31 as AD0.5
	LPC_GPIO1->FIOMASK3 |= 0xDF;  //P1.29(test LED)
	LPC_GPIO1->FIODIR3 |= 0x20;
	LPC_ADC->ADCR = 0x00210320;
	
	while(1)
	{
		while((LPC_ADC->ADSTAT & 0X00000020) != 0X00000020)
		{}
		
		adc=((LPC_ADC->ADDR5 >> 4) & 0x00000fff);
		
		if( adc > 0x4D9 )
		{
			LPC_GPIO1->FIOSET3=0x20;
		}
		else
		{
			LPC_GPIO1->FIOCLR3=0x20;
		}
	}
}
