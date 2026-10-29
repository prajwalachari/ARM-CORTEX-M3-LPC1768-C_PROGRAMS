/*
 * ============================================================================
 * File        : uart_rx_led_toggle.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0 & GPIO Port 1
 * Description : Serial remote command controller.
 *               Listens on UART0 for commands:
 *                 - ASCII '1' : Turns ON LED on P1.29
 *                 - ASCII '0' : Turns OFF LED on P1.29
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0
 *   P1.29 --> Controlled Output LED
 * ============================================================================
 */

#include <LPC17xx.h>

void uart0_init(void);
void delay(unsigned long int di);

int main(void) {
    unsigned long int val;
    SystemInit();
    
//    LPC_PINCON->PINSEL0 &= ~0x000000F0;
    LPC_PINCON->PINSEL0 |=  0x00000050;
    
    LPC_GPIO1->FIOMASK3 &= ~0x20;
    LPC_GPIO1->FIODIR3  |=  0x20;
    
    uart0_init();
    
    while(1) {
        while((LPC_UART0->LSR & 0x01) != 0x01) 
					{ 
            // Wait for data
        }
        val = LPC_UART0->RBR;
        
        if(val == '1') {
            LPC_GPIO1->FIOSET3 = 0x20; 
				}	
				else if (val == '0'){
            LPC_GPIO1->FIOCLR3 = 0x20; 
    }
	}
}

void uart0_init(void) { 
    LPC_SC->PCONP |= (1 << 3);
    
    LPC_SC->PCLKSEL0 &= ~(3 << 6);
    
    LPC_UART0->LCR = 0x83;
    LPC_UART0->DLM = 0x00;
    LPC_UART0->DLL = 0x75;
    LPC_UART0->FDR = 0x10; 
    LPC_UART0->LCR = 0x03;
}

void delay(unsigned long int di) {
    volatile unsigned long int i = 0;
    for(i = 0; i < di; i++) {
        // Loop execution
    }
}
