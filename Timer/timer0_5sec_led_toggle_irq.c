/*
 * ============================================================================
 * File        : timer0_5sec_led_toggle_irq.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : Timer0, NVIC, GPIO Port 1
 * Description : Precise periodic hardware timer interrupt.
 *               Initializes Timer0 with PR=24999 (1 ms per tick at 25MHz PCLK)
 *               and MR0=5000 (5-second period). On every match interrupt,
 *               TIMER0_IRQHandler toggles LED on P1.29 using XOR bitmask.
 * Pin Mapping :
 *   P1.29 --> Indicator LED (Toggles state every 5 seconds)
 * ============================================================================
 */

#include <LPC17xx.h>

#define LED_PIN (1 << 29) 

void GPIO_Init(void);
void Timer0_Init(void);

int main(void) {
    SystemInit();
    GPIO_Init();
    Timer0_Init();

    while(1) {
    }
}

void GPIO_Init(void) {
    LPC_PINCON->PINSEL3 &= ~(3 << 26);
    LPC_GPIO1->FIODIR |= LED_PIN;
    LPC_GPIO1->FIOCLR = LED_PIN;
}

void Timer0_Init(void) {
    LPC_SC->PCONP |= (1 << 1);
    LPC_SC->PCLKSEL0 &= ~(3 << 2);
    
    LPC_TIM0->TCR = 0x02; 
    LPC_TIM0->PR = 24999;
    LPC_TIM0->MR0 = 5000;
    LPC_TIM0->MCR = (1 << 0) | (1 << 1);
    LPC_TIM0->IR = 0xFF;
    
    NVIC_EnableIRQ(TIMER0_IRQn);
    LPC_TIM0->TCR = 0x01;
}

void TIMER0_IRQHandler(void) {
    if (LPC_TIM0->IR & (1 << 0)) {
        LPC_GPIO1->FIOPIN ^= LED_PIN;
        LPC_TIM0->IR = (1 << 0);
    }
}
