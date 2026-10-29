/*
 * ============================================================================
 * File        : uart_tx_string_9600.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0 (9600 Baud, 8-N-1)
 * Description : Serial string transmission program.
 *               Initializes UART0 at 9600 baud rate and transmits string
 *               " Prajwal Here!!" character-by-character checking THRE flag.
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0
 * ============================================================================
 */

#include <LPC17xx.h>
void delay(unsigned long int di);
void uart0_init() { 
    LPC_UART0->LCR = 0x83;
    LPC_UART0->DLM = 0x00;
    LPC_UART0->DLL = 0x75;
    LPC_UART0->FDR = 0x10; 
    LPC_UART0->LCR = 0x03;
}
int main()
	{
		unsigned char string[]= " Prajwal Here!!",i;
		SystemInit();
		LPC_PINCON->PINSEL0 |=0x00000050;
		
	  uart0_init();
		
		for(i=0;string[i]!='\0';i++)
		{
			LPC_UART0 ->THR = string[i];
			while((LPC_UART0->LSR&0X20)!=0X20)
			{
			}
		 delay(100000);
		
		}
		
	}
	

void delay(unsigned long int di) {
    volatile unsigned long int i = 0;
    for(i = 0; i < di; i++) {
    }
	}
