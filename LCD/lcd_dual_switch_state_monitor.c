/*
 * ============================================================================
 * File        : lcd_dual_switch_state_monitor.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0, Port 1, Port 2 (Parallel LCD & 2 Switches)
 * Description : Dual switch state machine with LCD status reporting.
 *               Reads SW1 (P1.25) and SW2 (P1.26). Evaluates current vs previous
 *               states and displays "BOTH LOW", "BOTH HIGH", or previous
 *               memory state on the LCD.
 * Pin Mapping :
 *   P0.21-P0.28 --> LCD D0-D7 Data Lines
 *   P2.11       --> LCD RS
 *   P2.13       --> LCD EN
 *   P1.25       --> Switch 1 Input
 *   P1.26       --> Switch 2 Input
 * ============================================================================
 */

#include "LPC17xx.h"

int sw1 = 0;
int sw2 = 0;
int prev_state = 0;

void delay(unsigned long int count)
{		unsigned long int i;
    for (i = 0; i < count; i++);}
void lcd_write(unsigned char ch, unsigned char type)
{
    LPC_GPIO0->FIOMASK = 0xE01FFFFF;
    LPC_GPIO0->FIOPIN  = ch * 0x00200000;
		if (type == 1)
        LPC_GPIO2->FIOSET = 0x00000800;
    else
        LPC_GPIO2->FIOCLR = 0x00000800;

    LPC_GPIO0->FIOCLR = 0x00000001;
		LPC_GPIO2->FIOSET = 0x00002000;
    delay(30000);
    LPC_GPIO2->FIOCLR = 0x00002000;
}
void lcd_init(void)
{
    unsigned char cmd[5] = {0x38, 0x0E, 0x06, 0x01, 0x80};
    int i;
    for (i = 0; i < 5; i++)
        lcd_write(cmd[i], 0);
}
void lcd_print(char *msg)
{
    int i = 0;
    while (msg[i] != '\0')
        lcd_write(msg[i++], 1);
}
int main(void)
{
    SystemInit();
	
		LPC_PINCON->PINSEL0 = 0x00000000;
    LPC_PINCON->PINSEL1 = 0x00000000;
    LPC_PINCON->PINSEL2 = 0x00000000;
    LPC_PINCON->PINSEL4 = 0x00000000;
		LPC_GPIO0->FIODIR |= 0x1FE00001;
    LPC_GPIO2->FIODIR |= 0x00002800;
    LPC_GPIO1->FIODIR &= ~(0x06000000);
		delay(50000);
    lcd_init();

    while (1)
    {
        sw1 = LPC_GPIO1->FIOPIN & 0x02000000;
        sw2 = LPC_GPIO1->FIOPIN & 0x04000000;
				if (sw1 != 0)
            sw1 = 1;
        else
            sw1 = 0;
				if (sw2 != 0)
            sw2 = 1;
        else
            sw2 = 0;
				lcd_write(0x80, 0);

        if (sw1 == 0 && sw2 == 0)
        {
            lcd_print("BOTH LOW");
            prev_state = 1;
        }
        else if (sw1 == 1 && sw2 == 1)
        {
            lcd_print("BOTH HIGH");
            prev_state = 0;
        }
        else
        {
            if (prev_state == 1)
                lcd_print("PREV LOW");
            else
                lcd_print("PREV HIGH");
        }
				delay(500000);
    }
}
