/*
 * ============================================================================
 * File        : lcd_8bit_relay_switch_monitor.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0, Port 1, Port 2 (8-bit Parallel LCD & Relay)
 * Description : Switch state monitor with relay actuation and LCD readout.
 *               Initializes 8-bit parallel LCD on P0.21-P0.28 with control
 *               lines on P2.11-P2.13. Polls input switch on P1.25:
 *                 - Switch ON:  Activates relay on P2.11, displays "INPUT: HIGH / RELAY: ON"
 *                 - Switch OFF: Deactivates relay, displays "INPUT: LOW / RELAY: OFF"
 * Pin Mapping :
 *   P0.21-P0.28 --> LCD Data D0-D7
 *   P2.11       --> Relay Output / LCD RS
 *   P2.12       --> LCD RW
 *   P2.13       --> LCD EN
 *   P1.25       --> Switch Digital Input
 * ============================================================================
 */

#include <LPC17xx.h>

void delay(unsigned long int x);
void lcdwrite(unsigned char ch);

int main()
{	
    unsigned long int i;
    unsigned char cmd[] = {0x38,0x0E, 0x06, 0x01, 0x80};
    unsigned char *msg = "welcome";//using pointers.
    unsigned char *msg1 = "welcome GOWDA";//using pointers.

    SystemInit();

    /* P0.21 - P0.24 as LCD data pins (D4-D7) */
    LPC_GPIO0->FIOMASK = 0xE01F0000;
    LPC_GPIO0->FIODIR  = 0x1FE00000;

    /* P2.11 = RS, P2.12 = RW, P2.13 = EN */
    LPC_GPIO2->FIOMASK1 = 0xC7;
    LPC_GPIO2->FIODIR1  = 0x38;
    LPC_GPIO2->FIOCLR1  = 0x38;      // RS=0, RW=0, EN=0

    delay(50000);
	
    for(i = 0; i < 5; i++)
    {
        lcdwrite(cmd[i]);
    }
		
		delay(50000);

    /* Display first line */
    LPC_GPIO2->FIOSET1 = 0x08;       // RS = 1

    while(*msg)//using pointers.
{
    lcdwrite(*msg);
    msg++;
}
    /* Move cursor to second line */
    LPC_GPIO2->FIOCLR1 = 0x08;       // RS = 0
    lcdwrite(0xC0);

    /* Display second line */
    LPC_GPIO2->FIOSET1 = 0x08;       // RS = 1

    while(*msg1) //using pointers.
{
    lcdwrite(*msg1);
    msg1++;
}

    while(1);
}

/*------------------------------------------------------*/

void lcdwrite(unsigned char ch)
{
    /* Clear old data */
    LPC_GPIO0->FIOCLR = 0x1FE00000;

    /* Send 8-bit data to P0.21-P0.28 */
    LPC_GPIO0->FIOSET = ((unsigned int)ch << 21);

    /* Enable Pulse */
    LPC_GPIO2->FIOSET = (1<<13);

    delay(500);

    LPC_GPIO2->FIOCLR = (1<<13);

    delay(5000);
}

/*------------------------------------------------------*/

void delay(unsigned long int x)
{
    unsigned long int i;

    for(i = 0; i < x; i++);
}




























//#include "LPC17xx.h"

//void delay(unsigned long int d)
//{
//    unsigned long int i;
//    for(i = 0; i < d; i++);
//}

//void lcd_pulse(void)
//{
//    LPC_GPIO0->FIOSET = 0x00000002;   // EN = 1 (P0.1)
//    delay(60000);
//    LPC_GPIO0->FIOCLR = 0x00000002;   // EN = 0
//}

//void lcd_cmd(unsigned char cmd)
//{
//    LPC_GPIO0->FIOCLR = 0x00000001;   // RS = 0
//    LPC_GPIO0->FIOCLR = 0x1FE00000;   // Clear data pins

//    // Manual bit mapping for P0.21 - P0.28
//    if(cmd & 0x01) LPC_GPIO0->FIOSET = 0x00200000;
//    if(cmd & 0x02) LPC_GPIO0->FIOSET = 0x00400000;
//    if(cmd & 0x04) LPC_GPIO0->FIOSET = 0x00800000;
//    if(cmd & 0x08) LPC_GPIO0->FIOSET = 0x01000000;
//    if(cmd & 0x10) LPC_GPIO0->FIOSET = 0x02000000;
//    if(cmd & 0x20) LPC_GPIO0->FIOSET = 0x04000000;
//    if(cmd & 0x40) LPC_GPIO0->FIOSET = 0x08000000;
//    if(cmd & 0x80) LPC_GPIO0->FIOSET = 0x10000000;

//    lcd_pulse();
//}

//void lcd_data(unsigned char data)
//{
//    LPC_GPIO0->FIOSET = 0x00000001;   // RS = 1
//    LPC_GPIO0->FIOCLR = 0x1FE00000;   // Clear data pins

//    if(data & 0x01) LPC_GPIO0->FIOSET = 0x00200000;
//    if(data & 0x02) LPC_GPIO0->FIOSET = 0x00400000;
//    if(data & 0x04) LPC_GPIO0->FIOSET = 0x00800000;
//    if(data & 0x08) LPC_GPIO0->FIOSET = 0x01000000;
//    if(data & 0x10) LPC_GPIO0->FIOSET = 0x02000000;
//    if(data & 0x20) LPC_GPIO0->FIOSET = 0x04000000;
//    if(data & 0x40) LPC_GPIO0->FIOSET = 0x08000000;
//    if(data & 0x80) LPC_GPIO0->FIOSET = 0x10000000;

//    lcd_pulse();
//}

//void lcd_print(char *s)
//{
//    while(*s)
//        lcd_data(*s++);
//}

//int main(void)
//{
//    SystemInit();

//    /* --- 1. PIN FUNCTION SELECT --- */
//    LPC_PINCON->PINSEL0 &= 0xFFFFFFF0;   // LCD Control (P0.0, P0.1)
//    LPC_PINCON->PINSEL1 &= 0xC0000000;   // LCD Data (P0.21-28)
//    LPC_PINCON->PINSEL4 &= 0xFF3FFFFF;   // Relay (P2.11)
//    
//    // P1.25 is controlled by PINSEL3. We clear bits 18 and 19.
//    LPC_PINCON->PINSEL3 &= 0xFFF3FFFF;   

//    /* --- 2. DIRECTION CONFIGURATION --- */
//    
//    // LCD (P0) and Relay (P2.11) are OUTPUTS -> Set to 1
//    LPC_GPIO0->FIODIR |= 0x1FE00003;     
//    LPC_GPIO2->FIODIR |= 0x00000800;     

//    // Switch (P1.25) is INPUT -> Set to 0
//    // We ensure the bit 25 is cleared (0)
//    LPC_GPIO1->FIODIR &= 0xFDFFFFFF;     

//    delay(200000);

//    /* LCD init */
//    lcd_cmd(0x38);
//    lcd_cmd(0x0C);
//    lcd_cmd(0x06);
//    lcd_cmd(0x01);
//    delay(200000);

//    while(1)
//    {
//        /* --- 3. READ INPUT --- */
//        
//        // Check if P1.25 is HIGH. 
//        // 0x02000000 is the hex value for the 25th bit.
//        if(LPC_GPIO1->FIOPIN & 0x02000000) 
//        {
//            /* --- LOGIC: SWITCH IS ON --- */
//            LPC_GPIO2->FIOSET = 0x00000800;   // Relay ON (P2.11 High)

//            lcd_cmd(0x80);                    // Line 1
//            lcd_print("INPUT: HIGH     ");
//            lcd_cmd(0xC0);                    // Line 2
//            lcd_print("RELAY: ON       ");
//        }
//        else
//        {
//            /* --- LOGIC: SWITCH IS OFF --- */
//            LPC_GPIO2->FIOCLR = 0x00000800;   // Relay OFF (P2.11 Low)

//            lcd_cmd(0x80);
//            lcd_print("INPUT: LOW      ");
//            lcd_cmd(0xC0);
//            lcd_print("RELAY: OFF      ");
//        }
//        
//        // Short delay to stabilize reading
//        delay(100000);
//    }
//}0
