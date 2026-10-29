/*
 * ============================================================================
 * File        : rfid_reader_uart_access_control_v2.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0 (9600 Baud) & GPIO Port 1
 * Description : Interactive RFID access management terminal.
 *               Prompts user on serial terminal, captures 13-digit RFID tag,
 *               performs database lookup, and prints:
 *               "-> MATCH! Access Granted." or "-> INVALID TAG! Access Denied."
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0
 *   P1.29 --> Output Status Indicator
 * ============================================================================
 */

#include <LPC17xx.h>
#include <string.h>

void uart0_init(void);
char uart0_rx(void);
void uart0_tx(char c);
void uart0_tx_string(char *str);
void delay(unsigned long int x);

int main(void)
{
    // 3 Authorized 13-character tags
    char authorized_cards[3][14] = {
        "7500398BBC7B",  
        "840085A99038",  
        "9876543210XYZ"   
    };
    
    char input_buffer[14]; 
    int i, j;
    int match_found;
		char c;

    SystemInit();

    // Configure UART0 and P1.29 (using pure hex as mandated)
    LPC_PINCON->PINSEL0 |= 0x00000050; 
    LPC_GPIO1->FIOMASK3 = 0xDF;        
    LPC_GPIO1->FIODIR3  = 0x20;        
    LPC_GPIO1->FIOCLR3  = 0x20;        

    uart0_init();

    while(1)
    {
        uart0_tx_string(" \r \n Scan (Type) 13-digit RFID Tag: ");

        // Read 13 characters from the terminal/simulator
        for(i = 0; i < 13; i++)
        {
            input_buffer[i] = uart0_rx();
            uart0_tx(input_buffer[i]); // Echo the character so you can see it
        }
        input_buffer[13] = '\0'; 
				if (c == '\r' || c == '\n')
					{
            break;
          }

        // Search logic
        match_found = 0; 
        for(j = 0; j < 3; j++) 
        {
            if (strcmp(input_buffer, authorized_cards[j]) == 0)
            {
                match_found = 1; 
                break;           
            }
        }

        // Output results to Terminal and virtual LED
        if (match_found == 1)
        {
            uart0_tx_string("\r\n-> MATCH! Access Granted.");
            LPC_GPIO1->FIOSET3 = 0x20;  
        }
        else
        {
            uart0_tx_string(" \r \n-> INVALID TAG! Access Denied.");
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

void delay(unsigned long int x)
{
    unsigned long int j;
    for(j = 0; j < x; j++);
}










































//#include <LPC17xx.h>
//#include <string.h>

//void uart0_init(void);
//char uart0_rx(void);
//void uart0_tx(char c);
//void uart0_tx_string(char *str);
//void delay(unsigned long int x);

//int main(void)
//{
//    
//    char authorized_card[13] = "09008207109C";
//		char input_buffer[13];

//    int i;
//    int match_found;

//    SystemInit();
//    LPC_PINCON->PINSEL0 |= 0x00000050;
//    LPC_PINCON->PINSEL1 &= ~(3 << 26);//txd0,rxd0
//    LPC_GPIO0->FIODIR |= (1 << 29);
//    LPC_GPIO1->FIOSET = (1 << 29);
//    uart0_init();

//    while(1)
//    {
//        uart0_tx_string("\r\nScan RFID Tag: ");
//				for(i = 0; i < 12; i++)
//        {
//            input_buffer[i] = uart0_rx();
//            uart0_tx(input_buffer[i]);
//        }
//        input_buffer[12] = '\0';
//        match_found = 0;
//				if(strcmp(input_buffer, authorized_card) == 0)
//        {
//            match_found = 1;
//        }
//        if(match_found == 1)
//        {
//            uart0_tx_string("\r\nDOOR OPEN");
//            LPC_GPIO1->FIOCLR = (1 << 29);
//            delay(4000000);
//            LPC_GPIO1->FIOSET = (1 << 29);
//						uart0_tx_string("\r\nDOOR CLOSED");
//        }
//        else
//        {
//            uart0_tx_string("\r\nACCESS DENIED");
//            LPC_GPIO1->FIOSET = (1 << 29);
//        }

//        delay(100000);
//    }
//}
//void uart0_init(void)
//{
//    LPC_UART0->LCR = 0x83;
//    LPC_UART0->DLM = 0x00;
//    LPC_UART0->DLL = 0x75;
//    LPC_UART0->FDR = 0x10;
//    LPC_UART0->LCR = 0x03;
//}
//char uart0_rx(void)
//{
//    while((LPC_UART0->LSR & 0x01) == 0)
//	  {}
//    return LPC_UART0->RBR;
//}

//void uart0_tx(char c)
//{
//    while((LPC_UART0->LSR & 0x20) == 0){
//		}
//    LPC_UART0->THR = c;
//}

//void uart0_tx_string(char *str)
//{
//    while(*str != '\0')
//    {
//        uart0_tx(*str);
//        str++;
//    }
//}
//void delay(unsigned long int x)
//{
//    unsigned long int j;

//    for(j = 0; j < x; j++)
//    {
//    }
//}
