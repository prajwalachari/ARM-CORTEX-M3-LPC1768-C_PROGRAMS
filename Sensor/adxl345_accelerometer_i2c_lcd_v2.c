/*
 * ============================================================================
 * File        : adxl345_accelerometer_i2c_lcd_v2.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : I2C0, ADXL345 Accelerometer, PCF8574 I2C 16x2 LCD
 * Description : ADXL345 3-axis accelerometer sensor interface (Version 2).
 *               Configures measure mode (POWER_CTL=0x08) and 100Hz data rate,
 *               reads burst acceleration registers 0x32-0x37, and updates
 *               X, Y, Z axes readings continuously on 16x2 I2C LCD.
 * Pin Mapping :
 *   P0.27 --> I2C0 SDA
 *   P0.28 --> I2C0 SCL
 * ============================================================================
 */

#include <LPC17xx.h>
#include <stdio.h>

#define LCD_ADDR 0x27       // LCD I2C address
#define ADXL_WRITE 0xA6     // ADXL345 Write address
#define ADXL_READ  0xA7     // ADXL345 Read address

void delay(unsigned long int d);
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ack);
void lcd_write(unsigned char ch, unsigned char rs);
void lcd_init(void);
void lcd_print(char *str);
void lcd_set_cursor(unsigned char row, unsigned char col);
void adxl345_init(void);
void adxl345_read(short *x, short *y, short *z);

int main()
{
    short x, y, z;
    char line1[16], line2[16];

    SystemInit();
    i2c_init();
    delay(0x20000);

    lcd_init();
    adxl345_init();

    while (1)
    {
        adxl345_read(&x, &y, &z);

        sprintf(line1, "X:%d Y:%d", x, y);
        sprintf(line2, "Z:%d", z);

        lcd_set_cursor(0, 0);
        lcd_print(line1);

        lcd_set_cursor(1, 0);
        lcd_print(line2);

        delay(0x80000);
    }
}

void delay(unsigned long int d)
{
    unsigned long int i;
    for (i = 0; i < d; i++);
}

void i2c_init(void)
{
    LPC_SC->PCONP |= 0x00000080;
    LPC_PINCON->PINSEL1 = 0x01400000;  // P0.27 SDA0, P0.28 SCL0
    LPC_I2C0->I2SCLH = 0x0000005A;
    LPC_I2C0->I2SCLL = 0x0000005A;
    LPC_I2C0->I2CONSET = 0x00000040;   // Enable I2C
}

void i2c_start(void)
{
    LPC_I2C0->I2CONSET = 0x00000020;
    while (LPC_I2C0->I2STAT != 0x08);
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
    while ((LPC_I2C0->I2CONSET & 0x08) == 0);
}

unsigned char i2c_read(unsigned char ack)
{
    if (ack)
        LPC_I2C0->I2CONSET = 0x00000004;
    else
        LPC_I2C0->I2CONCLR = 0x00000004;

    LPC_I2C0->I2CONCLR = 0x00000008;
    while ((LPC_I2C0->I2CONSET & 0x08) == 0x00);
    return LPC_I2C0->I2DAT;
}

/* ---------------- LCD FUNCTIONS ---------------- */
void lcd_write(unsigned char ch, unsigned char rs)
{
    unsigned char high_nib, low_nib;
    unsigned char data;

    high_nib = ch & 0xF0;
    low_nib = (ch << 4) & 0xF0;

    data = high_nib | (rs ? 0x01 : 0x00) | 0x08;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data | 0x04);
    delay(0x1000);
    i2c_write(data & ~0x04);
    delay(0x10000);
    i2c_stop();

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

void lcd_init(void)
{
    unsigned char cmd[] = {0x33, 0x32, 0x28, 0x0E, 0x01, 0x06, 0x80};
    unsigned char i;
    delay(0x10000);
    for (i = 0; i < 7; i++)
    {
        lcd_write(cmd[i], 0);
        delay(0x5000);
    }
}

void lcd_set_cursor(unsigned char row, unsigned char col)
{
    unsigned char pos;
    pos = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_write(pos, 0);
}

void lcd_print(char *str)
{
    while (*str)
    {
        lcd_write(*str++, 1);
        delay(0x1000);
    }
}

//ADXL FUNCTION
void adxl345_init(void)
{
    // DATA_FORMAT (0x31): a2g, full resolution
    i2c_start();
    i2c_write(ADXL_WRITE);
    i2c_write(0x31);
    i2c_write(0x08);
    i2c_stop();
    delay(0x5000);

    // BW_RATE (0x2C): 100Hz
    i2c_start();
    i2c_write(ADXL_WRITE);
    i2c_write(0x2C);
    i2c_write(0x0A);
    i2c_stop();
    delay(0x5000);

    // POWER_CTL (0x2D): Measure mode
    i2c_start();
    i2c_write(ADXL_WRITE);
    i2c_write(0x2D);
    i2c_write(0x08);
    i2c_stop();
    delay(0x10000);
}

void adxl345_read(short *x, short *y, short *z)
{
    unsigned char x0, x1, y0, y1, z0, z1;

    i2c_start();
    i2c_write(ADXL_WRITE);
    i2c_write(0x32);
    i2c_stop();

    i2c_start();
    i2c_write(ADXL_READ);
    x0 = i2c_read(1);
    x1 = i2c_read(1);
    y0 = i2c_read(1);
    y1 = i2c_read(1);
    z0 = i2c_read(1);
    z1 = i2c_read(0);
    i2c_stop();

    *x = (short)((x1 << 8) | x0);
    *y = (short)((y1 << 8) | y0);
    *z = (short)((z1 << 8) | z0);
}
