/*
 * ============================================================================
 * File        : adc_threshold_led_trigger_v2.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : ADC (AD0.5) & GPIO Port 1
 * Description : Analog threshold detection using burst/software start.
 *               Reads channel AD0.5 (P1.31). When ADC conversion completes
 *               (DONE bit high), checks against threshold 0x09B2 (~2.0V).
 *               Sets P1.29 HIGH when above threshold, LOW when below.
 * Pin Mapping :
 *   P1.31 --> AD0.5 (Analog Input)
 *   P1.29 --> Indicator LED (Output)
 * ============================================================================
 */

#include <LPC17xx.h>

unsigned short adc=0;

int main()
{
    SystemInit();

    LPC_SC->PCONP |= 0x00001000;
    LPC_PINCON->PINSEL3 |= 0xC0000000;//ad0.5 is at 30 and 31
    LPC_ADC->ADCR = 0x00200420;

    LPC_PINCON->PINSEL3 &= 0xCFFFFFFF;
    LPC_GPIO1->FIODIR |= 0x20000000;

    while(1)
    {
        LPC_ADC->ADCR |= 0x01000000;

        while((LPC_ADC->ADDR5 & 0x80000000)==0);

        adc = (LPC_ADC->ADDR5 >> 4) & 0x00000FFF;

        if(adc > 0x09B2)
            LPC_GPIO1->FIOSET = 0x20000000;
        else
            LPC_GPIO1->FIOCLR = 0x20000000;
    }
}
