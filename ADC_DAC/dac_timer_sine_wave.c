/*
 * ============================================================================
 * File        : dac_timer_sine_wave.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : 10-bit DAC (AOUT), Timer0, NVIC
 * Description : High-frequency sine wave generator using hardware Timer0
 *               interrupts and a 40-entry pre-computed lookup table.
 *               Timer0 triggers interrupt at 1 MHz, loading subsequent sine
 *               samples into DACR to synthesize a smooth sinusoidal signal.
 * Pin Mapping :
 *   P0.26 --> AOUT (Analog Sine Wave Output)
 * ============================================================================
 */

#include <LPC17xx.h>

#define LUT_SIZE 40

uint16_t sine_table[LUT_SIZE] = {
512,592,669,742,808,867,916,955,984,1002,
1010,1007,994,970,936,892,840,780,714,643,
568,492,417,343,274,209,151,100,59,28,
9,1,5,19,43,76,118,167,223,286
};

volatile int idx = 0;

void TIMER0_IRQHandler(void)
{
    LPC_TIM0->IR = 1;

    LPC_DAC->DACR = (sine_table[idx] << 6);

    idx++;
    if(idx >= LUT_SIZE)
        idx = 0;
}

int main(void)
{
    SystemInit();

    /* DAC pin */
    LPC_PINCON->PINSEL1 &= ~(3 << 20);
    LPC_PINCON->PINSEL1 |=  (2 << 20);

    /* Power Timer0 */
    LPC_SC->PCONP |= (1 << 1);

    LPC_TIM0->PR = 0;
    LPC_TIM0->MR0 = 17;   // 1 MHz update
    LPC_TIM0->MCR = 3;

    NVIC_EnableIRQ(TIMER0_IRQn);

    LPC_TIM0->TCR = 1;

    while(1);
}
