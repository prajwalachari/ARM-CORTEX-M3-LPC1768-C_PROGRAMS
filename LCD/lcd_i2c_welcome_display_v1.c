/*
 * ============================================================================
 * File        : lcd_i2c_welcome_display_v1.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : I2C0 Controller (100 kHz)
 * Description : Alphanumeric 16x2 LCD driver using PCF8574 I2C backpack.
 *               Initializes I2C0 at 100 kHz standard clock speed (SCLH=SCLL=90).
 *               Sends 4-bit nibble commands and data packets over I2C bus
 *               to display string "WELCOME".
 * Pin Mapping :
 *   P0.27 --> I2C0 SDA
 *   P0.28 --> I2C0 SCL
 * ============================================================================
 */

#include <LPC17xx.h>

#define LCD_ADDR 0x27 //here 0x27 is lcd display address

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
        lcd_write(cmd[i],0);  
        delay(0x5000);
    }


    for(i=0;msg1[i]!='\0';i++)
    {
        lcd_write(msg1[i],1); 
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
    LPC_SC->PCONP |= 0x00000080;      //power control for I2C0 is 7th pin //1000 0000       

    LPC_PINCON->PINSEL1 = 0x01400000;  //01 for mux to be select SDA &SCL in P0.27 and 28
	
			//data rate and duty cycle for standard freq 100KHz and 18Mhz for cpu
	
    LPC_I2C0->I2SCLH = 0x0000005A;          // SCL High time (90) for 100khz standard mode
    LPC_I2C0->I2SCLL = 0x0000005A;          // SCL Low time  (90) for 100khz standard mode
    LPC_I2C0->I2CONSET = 0x00000040;        // I2EN = 1 (Enable I2C)  //6th bit is fo enabling i2c interface
}


void i2c_start(void)
{
    LPC_I2C0->I2CONSET = 0x00000020;        // Set START bit  i.e., 5th bit
	  while(LPC_I2C0->I2STAT != 0x08)  // Wait for START transmitted //after start & interupt(SI)bit set on I2CONSET,status code is 0x08
		{
		}
    LPC_I2C0->I2CONCLR = 0x00000028;        // Clear SI //3rd bit is SI and 5th bit is START bit
}


void i2c_stop(void)
{
    LPC_I2C0->I2CONSET = 0x00000010;        // STOP bit   //4th bit is STOP bit 0001 0000
    LPC_I2C0->I2CONCLR = 0x00000008;        // Clear SI
    delay(0x2000);
}


void i2c_write(unsigned char data)
{
    LPC_I2C0->I2DAT = data;                 // Load data into I2DAT
    LPC_I2C0->I2CONCLR = 0x00000008;        // Clear SI i.e.,3rd bit gor interupt clear bit
    while((LPC_I2C0->I2CONSET & 0x00000008) == 0); // Wait till SI set  i.e., 3rd bit is inteript flag
}


void lcd_write(unsigned char ch, unsigned char rs)
{
    unsigned char high_nib, low_nib;
    unsigned char data;

    high_nib = ch & 0xF0;              // Upper nibble
    low_nib = (ch << 4) & 0xF0;        // Lower nibble

    // Send upper nibble first
    data = high_nib | (rs ? 0x01 : 0x00) | 0x08;   // 11th bit is RS in chosen ARM 
    i2c_start();
    i2c_write(LCD_ADDR << 1);          // Slave address + write //when left shifted lsb is 0 which is write operation
    i2c_write(data | 0x04);            // EN = 1   //13th bit is EN in chosen ARm
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
