/*
 * ============================================================================
 * File        : uart_echo_terminal_115200.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0 (115200 Baud, 8-N-1)
 * Description : High-speed serial communication terminal driver.
 *               Initializes UART0 for 115200 baud with fractional divider (FDR).
 *               Sends welcome banner, prompts user for text input, echoes
 *               typed characters, and prints back received string on enter.
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0
 * ============================================================================
 */

#include <LPC17xx.h>
void UART0_Init(void);
void UART0_SendChar(char ch);
void UART0_SendString(char *str);
char UART0_ReceiveChar(void);
void UART0_ReceiveString(char *buffer, int maxLength);


int main(void) {
    char inputBuffer[50]; 
    SystemInit();  
    UART0_Init();   
	  UART0_SendString("welcome to pesu!\r\n");

    while (1) {
        UART0_SendString("\r\nEnter text and press enter: ");
        UART0_ReceiveString(inputBuffer, 50); 
        UART0_SendString("\r\nYou typed: ");
        UART0_SendString(inputBuffer);
        UART0_SendString("\r\n");
    }
}

// --- FUNCTION DEFINITIONS ---

/**
 * Initializes UART0 to 115200 Baud assuming a 25MHz Peripheral Clock (PCLK).
 */
void UART0_Init(void) {
    // 1. Power up the UART0 peripheral block
    LPC_SC->PCONP |= (1 << 3); 

    // 2. Configure pins P0.2 as TXD0 and P0.3 as RXD0
    LPC_PINCON->PINSEL0 &= ~0x000000F0; // Clear bits 4,5,6,7
    LPC_PINCON->PINSEL0 |=  0x00000050; // Set pin functions to UART0

    // 3. Set Peripheral Clock (PCLK) for UART0 to CCLK/4 (25MHz if CCLK=100MHz)
    LPC_SC->PCLKSEL0 &= ~(3 << 6); // Bits 6:7 clear to 00

    // 4. Enable Divisor Latch Access Bit (DLAB = 1) to configure the baud rate
    // Also sets data format: 8-bit character length, 1 Stop bit, No Parity
    LPC_UART0->LCR = (1 << 7) | (3 << 0); 

    // 5. Load calculated fractional divisors for 115200 Baud
    LPC_UART0->DLM = 0;
    LPC_UART0->DLL = 13;                  // Integer component
    LPC_UART0->FDR = (16 << 4) | (9 << 0); // MULVAL = 16 (bits 7:4), DIVADDVAL = 9 (bits 3:0)

    // 6. DISABLE DLAB (DLAB = 0) to look up UnRBR and UnTHR registers
    LPC_UART0->LCR &= ~(1 << 7); 
}

/**
 * Transmits a single character through the UART0 Transmit Holding Register (THR).
 */
void UART0_SendChar(char ch) {
    // Poll line status register until Transmit Holding Register Empty (THRE, bit 5) is 1
    while (!(LPC_UART0->LSR & (1 << 5)));
    

    LPC_UART0->THR = ch;
}

void UART0_SendString(char *str) {
    while (*str != '\0') {
        UART0_SendChar(*str);
        str++;
    }
}

char UART0_ReceiveChar(void) {
    
    while (!(LPC_UART0->LSR & (1 << 0)));
    return LPC_UART0->RBR;
}

void UART0_ReceiveString(char *buffer, int maxLength) {
    char ch;
    int i = 0;


    while (i < (maxLength - 1)) {
        ch = UART0_ReceiveChar();
			  UART0_SendChar(ch); 
        if (ch == '\r' || ch == '\n') {
            break; 
        }

        buffer[i] = ch; // Store validated character byte inside the array
        i++;
    }
    
    buffer[i] = '\0'; // Null-terminate string array to comply with standard C styles
}
