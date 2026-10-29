/*
 * ============================================================================
 * File        : dac_square_wave_generation.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : 10-bit DAC (AOUT / P0.26)
 * Description : Square / rectangular wave generator via DAC.
 *               Toggles DAC register between maximum output (1023 << 6 = 3.3V)
 *               and minimum output (0 << 6 = 0V) with matched software delay.
 * Pin Mapping :
 *   P0.26 --> AOUT (Square Wave Output)
 * ============================================================================
 */

#include <LPC17xx.h>

int main(void)
{
    int d;
    
		LPC_PINCON->PINSEL1 |= (1<<21);

    while(1)
    {
      LPC_DAC->DACR = (1023 << 6);//high
				
			for(d=0; d<30000; d++){
			}
			
      LPC_DAC->DACR = (0 << 6);

      for(d=0; d<30000; d++){
			}//low
    }
}
