/*
 * ============================================================================
 * File        : adc_to_dac_passthrough.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : 12-bit ADC (AD0.5) & 10-bit DAC (AOUT)
 * Description : Direct ADC-to-DAC conversion loop.
 *               Samples analog input continuously on AD0.5 (P1.31), scales
 *               12-bit ADC result down to 10 bits (right-shift by 2), and
 *               immediately updates the hardware DAC register (P0.26) to
 *               reproduce the analog voltage in real time.
 * Pin Mapping :
 *   P1.31 --> AD0.5 (Analog Input)
 *   P0.26 --> AOUT / DAC (Analog Output)
 * ============================================================================
 */

#include <LPC17xx.h>

unsigned short adc;

int main()
{
    SystemInit();

    //ADC 

    LPC_SC->PCONP |= 0x00001000;
    LPC_PINCON->PINSEL3 |= 0xC0000000;  // AD0.5
    LPC_ADC->ADCR = 0x00200420;

    //DAC

    LPC_PINCON->PINSEL1 |= 0x00200000;  // P0.26 DAC

    while(1)
    {
        LPC_ADC->ADCR &= 0xF8FFFFFF;
        LPC_ADC->ADCR |= 0x01000000;

        while((LPC_ADC->ADDR5 & 0x80000000)==0);

        adc = (LPC_ADC->ADDR5 >> 4) & 0xFFF;

        /* 12-bit ADC and 10-bit DAC */
        adc = adc >> 2;

        LPC_DAC->DACR = (adc << 6);
    }
}
