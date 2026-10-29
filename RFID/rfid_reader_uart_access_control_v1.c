/*
 * ============================================================================
 * File        : rfid_reader_uart_access_control_v1.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0 (9600 Baud) & GPIO Port 1
 * Description : RFID reader interface and card authentication system.
 *               Reads 12-character ASCII tag ID from EM-18 RFID module over
 *               UART0, compares against authorized cards table, and triggers
 *               LED P1.29 for a timed duration upon valid card match.
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0 (Connected to EM-18 TX)
 *   P1.29 --> Access Granted Indicator LED / Solenoid
 * ============================================================================
 */

#include <LPC17xx.h>
#include <string.h>

void uart0_init(void);
char uart0_rx(void);
void delay(unsigned long int x);

int main(void)
{
    // Database updated with your specific 12-character tags.
    // The array is sized [13] to hold 12 characters + 1 Null Terminator.
    char authorized_cards[3][13] = {
        "090082070589",
        "090084336CD2",
        "09008206C944"
    };
    
    char input_buffer[13];
    int i, j;
    int match_found;

    SystemInit();

    // Pin configuration using pure hexadecimal mapping
    LPC_PINCON->PINSEL0 |= 0x00000050;
    LPC_GPIO1->FIOMASK3 = 0xDF;
    LPC_GPIO1->FIODIR3  = 0x20;
    LPC_GPIO1->FIOCLR3  = 0x20;

    uart0_init();

    while(1)
    {
        // Change: Read exactly 12 characters from the EM-18 module
        for(i = 0; i < 12; i++)
        {
            input_buffer[i] = uart0_rx();
        }
        // Change: Cap the array at index 12 with the Null Terminator
        input_buffer[12] = '\0';

        match_found = 0;
        
        for(j = 0; j < 3; j++) 
        {
            if (strcmp(input_buffer, authorized_cards[j]) == 0)
            {
                match_found = 1;
                break;
            }
        }

        if (match_found == 1)
        {
            LPC_GPIO1->FIOSET3 = 0x20;
            delay(500000);
            LPC_GPIO1->FIOCLR3 = 0x20;
        }
        else
        {
            LPC_GPIO1->FIOCLR3 = 0x20;
        }
        
        delay(100000);
    }
}

void uart0_init(void)
{
    LPC_UART0->LCR = 0x83;
    LPC_UART0->DLM = 0x00;
    LPC_UART0->DLL = 0x75;
    LPC_UART0->FDR = 0x10;
    LPC_UART0->LCR = 0x03;
}

char uart0_rx(void)
{
    while ((LPC_UART0->LSR & 0x01) == 0);
    return LPC_UART0->RBR;
}

void delay(unsigned long int x)
{
    unsigned long int j;
    for(j = 0; j < x; j++);
}
