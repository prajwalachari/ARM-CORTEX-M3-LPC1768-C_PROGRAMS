/*
 * ============================================================================
 * File        : uart_rx_password_auth_led.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0 & GPIO Port 1
 * Description : Password authentication terminal via serial console.
 *               Prompts user "Enter password: ", echoes typed characters,
 *               buffers up to newline, and validates against secret "hiNXP".
 *               If correct -> turns ON LED P1.29; if incorrect -> turns OFF.
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0
 *   P1.29 --> Indicator LED (High = Authenticated)
 * ============================================================================
 */

#include <LPC17xx.h>
#include <string.h> // Required for strcmp()

// Function prototypes
void uart0_init(void);
void delay(unsigned long int x);
void uart0_tx(char c);
void uart0_tx_string(char *str);
char uart0_rx(void);

int main(void)
{
    char expected_str[] = "hiNXP"; 
    char input_buffer[32];          
    int idx = 0;                   
    char c;

    SystemInit();

    
    LPC_PINCON->PINSEL0 |= 0x00000050;
		LPC_GPIO1->FIOMASK3 = 0XDF;//11011111
		LPC_GPIO1->FIODIR3 = 0X20;//00100000
		
    uart0_init();
    uart0_tx_string("Enter password: ");

    while(1)
    {
        c = uart0_rx();//receives single character
        uart0_tx(c);  //push char to terminal
        if (c == '\r' || c == '\n')
        {
            input_buffer[idx] = '\0'; 
            idx = 0;  //send to next input                
            if (strcmp(input_buffer, expected_str) == 0)
            {
                LPC_GPIO1-> FIOSET3 = 0X20;
            }
            else
            {
                LPC_GPIO1-> FIOCLR3 = 0X20 ;
            }
						//uart0_tx_string("\r\nEnter password: ");
        }
//        else if (c == '\b' && idx > 0) 
//        {
//            // Optional: Handle backspace
//            idx--;
//        }
        else if (idx < sizeof(input_buffer) - 1)
        {
            // Add character to buffer if there is space
            input_buffer[idx++] = c;
        }
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

//ftn to transmit single char
void uart0_tx(char c)
{
    
    while ((LPC_UART0->LSR & 0x20) == 0);
    LPC_UART0->THR = c;
}

void uart0_tx_string(char *str)
{
    while(*str != '\0')
    {
        uart0_tx(*str++);
    }
	}


	char uart0_rx(void)
	{
			
			while ((LPC_UART0->LSR & 0x01) == 0);
			return LPC_UART0->RBR; 
	}	`

void delay(unsigned long int x)
{
    unsigned long int j;
    for(j = 0; j < x; j++);
}
