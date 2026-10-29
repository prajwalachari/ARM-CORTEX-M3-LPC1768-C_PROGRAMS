/*
 * ============================================================================
 * File        : lcd_4bit_custom_string_display.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0 & Port 2 (4-bit LCD mode)
 * Description : 4-bit parallel LCD driver with dual-line text display.
 *               Displays "welcome" on line 1 and "Prajwal@123" on line 2.
 * Pin Mapping :
 *   P0.21-P0.24 --> LCD Data Nibbles
 *   P2.11       --> LCD RS
 *   P2.12       --> LCD RW
 *   P2.13       --> LCD EN
 * ============================================================================
 */

#include <LPC17xx.h>

void delay(unsigned long int x);
void lcdwrite(unsigned char ch);

int main()
{
    unsigned long int i;
    unsigned char cmd[] = {0x33, 0x32, 0x28, 0x0E, 0x06, 0x01, 0x80};
    unsigned char *msg = "welcome";//using pointers.
    unsigned char *msg1 = "Prajwal@123";//using pointers.

    SystemInit();

    
    LPC_GPIO0->FIOMASK = 0xFE1FFFFF;
    LPC_GPIO0->FIODIR  = 0x01E00000;
		LPC_GPIO2->FIOMASK1 = 0xC7;
    LPC_GPIO2->FIODIR1  = 0x38;
    LPC_GPIO2->FIOCLR1  = 0x38;    

    delay(50000);
		for(i = 0; i < 7; i++)
    {
        lcdwrite(cmd[i]);
    }
		
		delay(50000);
		LPC_GPIO2->FIOSET1 = 0x08;// RS

    while(*msg)
{
    lcdwrite(*msg);
    msg++;
}
   
    LPC_GPIO2->FIOCLR1 = 0x08;       // RS = 0
    lcdwrite(0xC0);
		LPC_GPIO2->FIOSET1 = 0x08;       // RS = 1

    while(*msg1) 
{
    lcdwrite(*msg1);
    msg1++;
}

    while(1);
}
void lcdwrite(unsigned char ch)
{
    unsigned char upper, lower;

    
    upper = ch & 0xF0;
    LPC_GPIO0->FIOPINH = upper << 1;

    LPC_GPIO2->FIOSET1 = 0x20;  
    delay(500);
    LPC_GPIO2->FIOCLR1 = 0x20; 
    delay(5000);

    
    lower = ch & 0x0F;
    LPC_GPIO0->FIOPINH = lower << 5;

    LPC_GPIO2->FIOSET1 = 0x20;  
    delay(500);
    LPC_GPIO2->FIOCLR1 = 0x20; 
    delay(5000);
}
void delay(unsigned long int x)
{
    unsigned long int i;

    for(i = 0; i < x; i++);
}
