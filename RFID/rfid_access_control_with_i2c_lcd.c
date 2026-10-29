/*
 * ============================================================================
 * File        : rfid_access_control_with_i2c_lcd.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0, I2C0, 16x2 LCD (PCF8574), GPIO Port 1
 * Description : Complete standalone RFID door lock / security access system.
 *               Displays status on 16x2 I2C LCD ("RFID READY").
 *               When an RFID card is swiped at EM-18 reader:
 *                 - Reads 12-byte card ID via UART0
 *                 - Displays Card ID on Line 2
 *                 - If authorized -> Displays "ACCESS GRANTED", activates P1.29
 *                 - If unauthorized -> Displays "ACCESS DENIED", keeps P1.29 OFF
 * Pin Mapping :
 *   P0.2  --> UART0 TXD0
 *   P0.3  --> UART0 RXD0 (from RFID Reader)
 *   P0.27 --> I2C0 SDA (LCD)
 *   P0.28 --> I2C0 SCL (LCD)
 *   P1.29 --> Door Lock / Authorization LED (Output)
 * ============================================================================
 */

#include <LPC17xx.h>
#include <string.h>
#define LCD_ADDR 0x27
void delay(unsigned long int d);
void uart0_init(void);
char uart0_rx(void);
void uart0_tx(char ch);
void uart0_tx_string(char *str);

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);


void lcd_write(unsigned char ch, unsigned char rs);
void lcd_string(char *str);
void lcd_cmd(unsigned char cmd);


int main(void)
{
    char authorized_cards[3][13] =
    {
        "090082070589",
        "090084336CD2",
        "09008206C944"
    };
		char input_buffer[13];
		unsigned char cmd[] ={0x33,0x32,0x28,0x0E,0x01,0x06,0x80};
		unsigned char i;
    int j;
    int match_found;

    SystemInit();
    LPC_GPIO1->FIOMASK3 = 0xDF;
    LPC_GPIO1->FIODIR3  = 0x20;
    LPC_GPIO1->FIOCLR3  = 0x20;

    LPC_PINCON->PINSEL0 |= 0x00000050;

    uart0_init();
		i2c_init();
    delay(0x100);

    for(i = 0; i < 7; i++)
    {
        lcd_write(cmd[i], 0);
        delay(0x5000);
    }
    lcd_cmd(0x80);
    lcd_string("RFID READY");

    delay(0x50000);
    lcd_cmd(0x01);
    delay(0x5000);

    while(1)
    {
        for(i = 0; i < 12; i++)
        {
            input_buffer[i] = uart0_rx();
            uart0_tx(input_buffer[i]);
        }
        input_buffer[12] = '\0';
        uart0_tx('\r');
        uart0_tx('\n');
        match_found = 0;

        for(j = 0; j < 3; j++)
        {
            if(strcmp(input_buffer, authorized_cards[j]) == 0)
            {
                match_found = 1;
                break;
            }
        }
        lcd_cmd(0x01);       // Clear LCD
        delay(0x5000);
        lcd_cmd(0x80);       // First line
        lcd_string("RFID:");
        lcd_cmd(0xC0);       // Second line
        lcd_string(input_buffer);
        if(match_found == 1)
        {
            LPC_GPIO1->FIOSET3 = 0x20;
            uart0_tx_string("AUTHORIZED\r\n");
          
            delay(0x50000);
            lcd_cmd(0x01);
            delay(0x5000);
            lcd_cmd(0x80);
            lcd_string("ACCESS GRANTED");
            delay(500000);
            LPC_GPIO1->FIOCLR3 = 0x20;
        }

        else
        {
            LPC_GPIO1->FIOCLR3 = 0x20;
            uart0_tx_string("UNAUTHORIZED\r\n");
            delay(0x50000);
            lcd_cmd(0x01);
            delay(0x5000);
            lcd_cmd(0x80);
            lcd_string("ACCESS DENIED");
        }
        delay(100000);
        lcd_cmd(0x01);
        delay(0x5000);
        lcd_cmd(0x80);
        lcd_string("RFID READY");
    }
}
void delay(unsigned long int d)
{
    unsigned long int i;

    for(i = 0; i < d; i++);
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
    while((LPC_UART0->LSR & 0x01) == 0)
    {}
    return LPC_UART0->RBR;
}
void uart0_tx(char ch)
{
    while((LPC_UART0->LSR & 0x20) == 0){
		}
    LPC_UART0->THR = ch;
}
void uart0_tx_string(char *str)
{
    while(*str != '\0')
    {
        uart0_tx(*str);
        str++;
    }
}
void i2c_init(void)
{
    LPC_SC->PCONP |= 0x00000080;
    LPC_PINCON->PINSEL1 = 0x01400000;
    LPC_I2C0->I2SCLH = 0x0000005A;
    LPC_I2C0->I2SCLL = 0x0000005A;
    LPC_I2C0->I2CONSET = 0x00000040;
}

void i2c_start(void)
{
    LPC_I2C0->I2CONSET = 0x00000020;
    while(LPC_I2C0->I2STAT != 0x08);
    LPC_I2C0->I2CONCLR = 0x00000028;
}
void i2c_stop(void)
{
    LPC_I2C0->I2CONSET = 0x00000010;
    LPC_I2C0->I2CONCLR = 0x00000008;
    delay(0x2000);
}
void i2c_write(unsigned char data)
{
    LPC_I2C0->I2DAT = data;
    LPC_I2C0->I2CONCLR = 0x00000008;
    while((LPC_I2C0->I2CONSET & 0x00000008) == 0);
}
void lcd_write(unsigned char ch, unsigned char rs)
{
    unsigned char high_nib;
    unsigned char low_nib;
    unsigned char data;
    high_nib = ch & 0xF0;
    data = high_nib |(rs ? 0x01 : 0x00) |0x08;
		i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data | 0x04);
    delay(0x1000);
    i2c_write(data & ~0x04);
    delay(0x10000);
    i2c_stop();
    low_nib = (ch << 4) & 0xF0;
    data = low_nib | (rs ? 0x01 : 0x00) | 0x08;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data | 0x04);
    delay(0x1000);
    i2c_write(data & ~0x04);
    delay(0x10000);
    i2c_stop();
    delay(0x3000);
}
void lcd_string(char *str)
{
    while(*str != '\0')
    {
        lcd_write(*str, 1);

        str++;
    }
}
void lcd_cmd(unsigned char cmd)
{
    lcd_write(cmd, 0);

    delay(0x5000);
}
