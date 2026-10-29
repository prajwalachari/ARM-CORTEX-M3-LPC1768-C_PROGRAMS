/*
 * ============================================================================
 * File        : uart_tx_char_sequence.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0 (9600 Baud, 8-N-1)
 * Description : Transmits string " ARM Cortex M3" over UART0 serial port
 *               at 9600 baud with pacing delays between consecutive characters.
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0
 * ============================================================================
 */

#include <LPC17xx.h>

void uart0_init(void);
void delay(unsigned long int x);

int main(void)
{
    unsigned char str1[] = " ARM Cortex M3";
    unsigned int i;

    SystemInit();

    /* Configure P0.2 as TXD0 and P0.3 as RXD0 */
    LPC_PINCON->PINSEL0 |= 0x00000050;

    uart0_init();

    for(i = 0; str1[i] != '\0'; i++)
    {
        LPC_UART0->THR = str1[i];

        /* Wait until THR becomes empty */
        while ((LPC_UART0->LSR & 0x20) == 0);

        delay(100000);
    }

    while(1);

    // return 0;   // not required in embedded applications
}

void uart0_init(void)
{
    LPC_UART0->LCR = 0x83;     // DLAB = 1

    LPC_UART0->DLM = 0x00;
    LPC_UART0->DLL = 0x75;

    LPC_UART0->FDR = 0x10;

    LPC_UART0->LCR = 0x03;     // 8-bit, No parity, 1 stop bit
}

void delay(unsigned long int x)
{
    unsigned long int j;

    for(j = 0; j < x; j++)
    {
        ;
    }
}
