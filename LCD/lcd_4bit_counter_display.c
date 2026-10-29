/*
 * ============================================================================
 * File        : lcd_4bit_counter_display.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0 (4-bit LCD interface)
 * Description : 4-bit parallel HD44780 LCD driver.
 *               Sends upper and lower nibbles with enable pulses.
 *               Initializes LCD and prints decimal counter loop from 1 to 16.
 * Pin Mapping :
 *   P0.4-P0.7 --> LCD D4-D7 Data Lines
 *   P0.0      --> LCD RS
 *   P0.1      --> LCD RW
 *   P0.2      --> LCD EN
 * ============================================================================
 */

#include <LPC17xx.h>

unsigned int ctrl[3] = {0x01,0x02,0x04};
unsigned char cmd_arr[] = {0x28,0x0C,0x06,0x01,0x80};

void dly(unsigned int x)
{
    unsigned int i;
    for(i=0;i<x*3000;i++);
}

void pulse(void)
{
    LPC_GPIO0->FIOPIN |= ctrl[2];
    dly(1);
    LPC_GPIO0->FIOPIN &= ~ctrl[2];
}

void send(unsigned char v, unsigned char t)
{
    if(t==0) LPC_GPIO0->FIOPIN &= ~ctrl[0];
    else     LPC_GPIO0->FIOPIN |=  ctrl[0];

    LPC_GPIO0->FIOPIN &= ~ctrl[1];

    LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~0xF0) | (v & 0xF0);
    pulse();

    LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~0xF0) | ((v<<4) & 0xF0);
    pulse();

    dly(2);
}

int main(void)
{
    LPC_GPIO0->FIODIR |= 0x000000F7;

    dly(20);

    send(0x30,0); dly(5);
    send(0x30,0); dly(5);
    send(0x30,0); dly(5);
    send(0x20,0);

    int i;

    for(i=0;i<5;i++) send(cmd_arr[i],0);

    for(i=1;i<=16;i++)
    {
        if(i>=10) send('1',1);
        send((i%10)+'0',1);
    }

    while(1);
}
