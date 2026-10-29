/*
 * ============================================================================
 * File        : dac_ramp_staircase_wave.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : 10-bit DAC (AOUT)
 * Description : Generates an analog staircase/ramp waveform using the
 *               internal Digital-to-Analog Converter (DAC). Sweeps DACR
 *               from 0 to 1023 repeatedly with incremental software delay.
 * Pin Mapping :
 *   P0.26 --> AOUT (Analog Voltage Output)
 * ============================================================================
 */

/*Simple DAC Ramp ,Output - staircase waveform.*/
#include <LPC17xx.h>

void delay(unsigned int d)
{
    while(d--);
}

int main()
{
    unsigned int i;

    SystemInit();

    LPC_PINCON->PINSEL1 |= 0x00200000;

    while(1)
    {
        for(i=0;i<1024;i++)
        {
            LPC_DAC->DACR = (i << 6);
            delay(1000);
        }
    }
}
