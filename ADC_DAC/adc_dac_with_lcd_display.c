/*
 * ============================================================================
 * File        : adc_dac_with_lcd_display.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : ADC (AD0.5), DAC (AOUT), 4-bit Parallel LCD (Port 0)
 * Description : Complete ADC-DAC measurement and display workstation.
 *               Reads 12-bit ADC value from P1.31, translates it to 10-bit
 *               DAC output on P0.26, converts numerical values to decimal
 *               strings, and updates Line 1 (ADC value) and Line 2 (DAC value)
 *               on a 16x2 HD44780 LCD display in 4-bit mode.
 * Pin Mapping :
 *   P1.31 --> AD0.5 (Analog In)
 *   P0.26 --> DAC AOUT (Analog Out)
 *   P0.4-P0.7 --> LCD D4-D7 Data Lines
 *   P0.10 --> LCD RS
 *   P0.11 --> LCD EN
 * ============================================================================
 */

#include <LPC17xx.h>

unsigned short adc;
char adc_str[16],dac_str[16];

void delay()
{
    unsigned int i;
    for(i=0;i<0x3FFF;i++);
}

void lcd_cmd(unsigned char cmd)
{
    LPC_GPIO0->FIOPIN = (cmd & 0xF0);
    LPC_GPIO0->FIOCLR = 0x00000400;
    LPC_GPIO0->FIOSET = 0x00000800;
    delay();
    LPC_GPIO0->FIOCLR = 0x00000800;

    LPC_GPIO0->FIOPIN = ((cmd<<4) & 0xF0);
    LPC_GPIO0->FIOSET = 0x00000800;
    delay();
    LPC_GPIO0->FIOCLR = 0x00000800;
}

void lcd_data(unsigned char data)
{
    LPC_GPIO0->FIOPIN = (data & 0xF0);
    LPC_GPIO0->FIOSET = 0x00000400;
    LPC_GPIO0->FIOSET = 0x00000800;
    delay();
    LPC_GPIO0->FIOCLR = 0x00000800;

    LPC_GPIO0->FIOPIN = ((data<<4) & 0xF0);
    LPC_GPIO0->FIOSET = 0x00000800;
    delay();
    LPC_GPIO0->FIOCLR = 0x00000800;
}

void lcd_string(char *p)
{
    while(*p)
        lcd_data(*p++);
}

void lcd_init()
{
    LPC_GPIO0->FIODIR |= 0x00000FF0;

    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

void int_to_str(unsigned int val,char *str)
{
    int i=0,j;
    char temp[10];

    if(val==0)
    {
        str[0]='0';
        str[1]='\0';
        return;
    }

    while(val)
    {
        temp[i++] = val%10 + '0';
        val/=10;
    }

    for(j=0;j<i;j++)
        str[j]=temp[i-j-1];

    str[i]='\0';
}

int main()
{
    SystemInit();

    /* ---------- ADC ---------- */

    LPC_SC->PCONP |= 0x00001000;
    LPC_PINCON->PINSEL3 |= 0xC0000000;
    LPC_ADC->ADCR = 0x00200420;

    /* ---------- DAC ---------- */

    LPC_PINCON->PINSEL1 |= 0x00200000;

    lcd_init();

    while(1)
    {
        LPC_ADC->ADCR &= 0xF8FFFFFF;
        LPC_ADC->ADCR |= 0x01000000;

        while((LPC_ADC->ADDR5 & 0x80000000)==0);

        adc = (LPC_ADC->ADDR5 >> 4) & 0xFFF;

        adc = adc >> 2;

        LPC_DAC->DACR = (adc << 6);

        int_to_str(adc,adc_str);
        int_to_str((adc<<6),dac_str);

        lcd_cmd(0x80);
        lcd_string("ADC:");
        lcd_string(adc_str);

        lcd_cmd(0xC0);
        lcd_string("DAC:");
        lcd_string(dac_str);

        delay();
    }
}
