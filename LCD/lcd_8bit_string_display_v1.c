/*
 * ============================================================================
 * File        : lcd_8bit_string_display_v1.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0 & Port 1 (8-bit Parallel LCD)
 * Description : 8-bit parallel HD44780 LCD character display.
 *               Data bus connected to P1.19-P1.26.
 *               Control lines: RS=P0.4, RW=P0.5, EN=P0.6.
 *               Displays text banner on LCD line 1.
 * Pin Mapping :
 *   P1.19-P1.26 --> LCD Data Lines D0-D7
 *   P0.4        --> LCD RS
 *   P0.5        --> LCD RW
 *   P0.6        --> LCD EN
 * ============================================================================
 */

#include <LPC17xx.h>
void delay (unsigned long int x)
{
	unsigned long int i;
	for (i=0;i<x;i++)
	{}
		
}
		int main()
		{
			unsigned char cmd[]={0x38,0x0E,0x06,0x01,0x80};
			unsigned char str[]="MANOJ IS A MAN",i;
			
			
			SystemInit();
			LPC_GPIO1->FIOMASKH=~(0x00FF<<3);//p1.19 to p1.26
			LPC_GPIO1->FIODIRH=(0x00FF<<3);
			
			
			LPC_GPIO0->FIOMASK=~(0x70);
			LPC_GPIO0->FIODIR=(0x70);
		
			LPC_GPIO0->FIOCLR=(0x10);//rs=p0.4
			LPC_GPIO0->FIOCLR=(0X20);//rw=p0.5
			
		
			for(i=0;i<5;i++)
			{
				LPC_GPIO1->FIOPINH=cmd[i]<<3;
				LPC_GPIO0->FIOSET=(0X40);//en=p0.6
				delay(50000);
				LPC_GPIO0->FIOCLR=(0x40);
				delay(50000);
			}
				LPC_GPIO0->FIOSET=(0x10);
			for(i=0;str[i]!='\0';i++)
			{
				LPC_GPIO1 ->FIOPINH=str[i]<<3;
				LPC_GPIO0 ->FIOSET=(0X40);
				delay(50000);
				LPC_GPIO0 ->FIOCLR=(0x40);
				delay(50000);
				
			}
		}





























































/*#include <LPC17xx.H>
#define RS  (1 << 13) // P2.13
#define RW  (1 << 12) // P2.12
#define EN  (1 << 11) // P2.11

void delay(unsigned long int x);
void lcdgpioinit(void);
void lcdwrt(unsigned char y);

int main(void)
{
    unsigned char cmd[] = {0x38, 0x0E, 0x06, 0x01, 0x80};
    unsigned char msg[] = "PES UNIVERSITY prajwal";
    unsigned char i;
		SystemInit();
   
		LPC_GPIO2->FIOCLR = RW;
    LPC_GPIO2->FIOCLR = RS; 
		for (i = 0; i < 5; i++)
    {
        lcdwrt(cmd[i]);
    }
		LPC_GPIO2->FIOSET = RS;
		lcdgpioinit();
    for (i = 0; msg[i] != '\0'; i++)
    {
        lcdwrt(msg[i]);
    }
}

void lcdwrt(unsigned char y)
{
    
    LPC_GPIO1->FIOCLR = (0xFF << 19);
    LPC_GPIO1->FIOSET = ((unsigned long int)y << 19);
    LPC_GPIO2->FIOSET = EN;
    delay(10);
    LPC_GPIO2->FIOCLR = EN;
    delay(1000);
}

void lcdgpioinit(void)
{
   
    LPC_GPIO2->FIOMASK &= ~(RS | RW | EN);
    LPC_GPIO1->FIOMASK &= ~(0xFF << 19);
	  LPC_GPIO2->FIODIR |= (RS | RW | EN);
    LPC_GPIO1->FIODIR |= (0xFF << 19);
}

void delay(unsigned long int x)
{
    unsigned long int j;
    for (j = 0; j < x; j++);
}*/
