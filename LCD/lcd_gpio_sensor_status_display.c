/*
 * ============================================================================
 * File        : lcd_gpio_sensor_status_display.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0 & Port 1, 8-bit Parallel LCD
 * Description : Digital sensor monitor with real-time LCD notification.
 *               Reads sensor signal on P0.19. When sensor state changes,
 *               updates LCD line 2 with "HIGH" or "LOW " dynamically.
 * Pin Mapping :
 *   P1.19-P1.26 --> LCD D0-D7 Data
 *   P0.4        --> LCD RS
 *   P0.5        --> LCD RW
 *   P0.6        --> LCD EN
 *   P0.19       --> Digital Sensor Input
 * ============================================================================
 */

#include <LPC17xx.h>

void delay(unsigned long int x)
{
    unsigned long int i;
    for(i = 0; i < x; i++)
    {
    }
}
void lcd_cmd(unsigned char cmd)
{
    LPC_GPIO0->FIOCLR = (1 << 4); //rs = 0
    LPC_GPIO0->FIOCLR = (1 << 5);//rw =0

    LPC_GPIO1->FIOPINH = cmd << 3;   
	
    LPC_GPIO0->FIOSET = (1 << 6);      
    delay(10000);
    LPC_GPIO0->FIOCLR = (1 << 6); 
    delay(10000);
}
void lcd_data(unsigned char data)
{
    LPC_GPIO0->FIOSET = (1 << 4); 
    LPC_GPIO0->FIOCLR = (1 << 5); 

    LPC_GPIO1->FIOPINH = data << 3;
    LPC_GPIO0->FIOSET = (1 << 6);
    delay(10000);
    LPC_GPIO0->FIOCLR = (1 << 6);
    delay(10000);``
}

void lcd_string(unsigned char *str)
{
    unsigned char i;
    for(i = 0; str[i] != '\0'; i++)
    {
        lcd_data(str[i]);
    }
}

int main( )
{
   
    unsigned char cmd[] = {0x38, 0x0C, 0x06, 0x01, 0x80};
    unsigned char i;
    int last_state;
    int current_state;

    last_state = -1;

    SystemInit();

    
    LPC_GPIO1->FIOMASKH &= ~(0x00FF << 3);
    LPC_GPIO1->FIODIRH  |=  (0x00FF << 3);
    LPC_GPIO0->FIOMASK &= ~(0x70 | (1 << 19));
    LPC_GPIO0->FIODIR  |= 0x70;
    LPC_GPIO0->FIODIR  &= ~(1 << 19);
    LPC_PINCON->PINMODE1 &= ~(3 << 6);
    for(i = 0; i < 5; i++)
    {
        lcd_cmd(cmd[i]);
        delay(20000);
    }

    lcd_cmd(0x80);
    lcd_string((unsigned char *)"SENSOR STATUS");

    while(1)
    {
        if((LPC_GPIO0->FIOPIN & (1 << 19)) == 0)
        {
            current_state = 1;
        }
        else
        {
            current_state = 0;
        }

        if(current_state != last_state)
        {
            lcd_cmd(0xC0);
            if(current_state == 1)
            {
                lcd_string((unsigned char *)"HIGH");
            }
            else
            {
                lcd_string((unsigned char *)"LOW ");
            }
            last_state = current_state;
            delay(50000); 
        }
    }
}
