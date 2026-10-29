/*
 * ============================================================================
 * File        : lcd_i2c_welcome_display_v2.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : I2C0 Controller (100 kHz)
 * Description : 16x2 LCD interface using PCF8574 I2C expander.
 *               Implements I2C start, stop, and byte transmit routines.
 *               Transmits initialization sequence in 4-bit mode and prints
 *               message "WELCOME" on display.
 * Pin Mapping :
 *   P0.27 --> I2C0 SDA
 *   P0.28 --> I2C0 SCL
 * ============================================================================
 */

#include <LPC17xx.h>

#define LCD_ADDR 0x27 

void delay(unsigned long int d);
void lcd_write(unsigned char ch, unsigned char rs);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
void i2c_init(void);

int main()
{
    unsigned char cmd[]={0x33,0x32,0x28,0x0E,0x01,0X06,0x80};
    unsigned char msg1[]="WELCOME";
    unsigned char i;

    SystemInit();
    i2c_init();

		delay(0x100);


    for(i=0;i<7;i++)
    {
        lcd_write(cmd[i],0);  //rs=0
        delay(0x5000);
    }


    for(i=0;msg1[i]!='\0';i++)
    {
        lcd_write(msg1[i],1);  //rs=1
        delay(0x8000);
    }

    while(1);
}

void delay(unsigned long int d)
{
    unsigned long int i;
    for(i=0;i<d;i++);
}

void i2c_init(void)
{
    LPC_SC->PCONP |= 0x00000080;//7
    LPC_PINCON->PINSEL1 = 0x01400000;//p0.27 28 - sda,scl

    LPC_I2C0->I2SCLH = 0x0000005A;// SCL High time (90) for 100khz standard mode
    LPC_I2C0->I2SCLL = 0x0000005A;// SCL Low time  (90) for 100khz standard mode
    LPC_I2C0->I2CONSET = 0x00000040;// I2EN = 1 (Enable I2C) 6
}


void i2c_start(void)
{
    LPC_I2C0->I2CONSET = 0x00000020;        // Set START bit
	  while(LPC_I2C0->I2STAT != 0x08);        // Wait for START transmitted
    LPC_I2C0->I2CONCLR = 0x00000028;        // Clear SI
}


void i2c_stop(void)
{
    LPC_I2C0->I2CONSET = 0x00000010;        // STOP bit
    LPC_I2C0->I2CONCLR = 0x00000008;        // Clear SI
    delay(0x2000);
}


void i2c_write(unsigned char data)
{
    LPC_I2C0->I2DAT = data;                 // Load data into I2DAT
    LPC_I2C0->I2CONCLR = 0x00000008;        // Clear SI
    while((LPC_I2C0->I2CONSET & 0x00000008) == 0); // Wait till SI set
}


void lcd_write(unsigned char ch, unsigned char rs)
{
    unsigned char high_nib, low_nib;
    unsigned char data;

    high_nib = ch & 0xF0;              // Upper nibble
    low_nib = (ch << 4) & 0xF0;        // Lower nibble

    // Send upper nibble first
    data = high_nib | (rs ? 0x01 : 0x00) | 0x08  ; 
    i2c_start();
    i2c_write(LCD_ADDR << 1);          // Slave address + write
    i2c_write(data | 0x04);            // EN = 1
	  delay(0x1000);
    i2c_write(data & ~0x04);           // EN = 0
		delay(0x10000);
    i2c_stop();

    // Send lower nibble
    data = low_nib | (rs ? 0x01 : 0x00) | 0x08;   
    i2c_start();
    i2c_write(LCD_ADDR << 1);          // Slave address + write
    i2c_write(data | 0x04);            // EN = 1
		delay(0x1000);
    i2c_write(data & ~0x04);           // EN = 0
		delay(0x10000);
    i2c_stop();

    delay(0x3000);
}
