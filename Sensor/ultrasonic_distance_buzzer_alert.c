/*
 * ============================================================================
 * File        : ultrasonic_distance_buzzer_alert.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 1 & Port 2, HC-SR04 Ultrasonic Sensor, Buzzer
 * Description : Obstacle distance detection with active audible buzzer alarm.
 *               Sends 10us trigger pulse on P2.11, measures duration of echo
 *               pulse on P2.12, and calculates distance in centimeters.
 *               If obstacle distance < 50 cm, sounds active buzzer on P1.29.
 * Pin Mapping :
 *   P2.11 --> Ultrasonic TRIG (Output)
 *   P2.12 --> Ultrasonic ECHO (Input)
 *   P1.29 --> Audible Buzzer / Alarm LED (Output)
 * ============================================================================
 */

#include <LPC17xx.h>

void delay(unsigned long int d);
unsigned int measure_distance(void);

int main()
{
    unsigned int distance;

    SystemInit();

    // ----- Configure TRIG (P2.11) and ECHO (P2.12) -----
    LPC_GPIO2->FIOMASK1 = 0xE7;      // Unmask bits 3 & 4
    LPC_GPIO2->FIODIR1 = 0X08;      // TRIG output // ECHO input

    // ----- Configure Buzzer (P1.29) -----
    LPC_GPIO1->FIOMASK3 = 0xDF;      // Allow bit5 
    LPC_GPIO1->FIODIR3 |= 0X20;    // Buzzer output
    LPC_GPIO1->FIOCLR3 = 0X20;     // Buzzer OFF initially

    while(1)
    {
        distance = measure_distance();

        if(distance < 50)
				{LPC_GPIO1->FIOSET3 = 0X20; }  // Buzzer ON
        else
				{ LPC_GPIO1->FIOCLR3 = 0X20;   // Buzzer OFF
			}
        delay(0xFFFFF);
    }
}

void delay(unsigned long int d)
{ unsigned long int i;
    for(i=0; i<d; i++);
}

unsigned int measure_distance(void)
{
    unsigned int time = 0;

    // ----- TRIGGER 10us PULSE -----
    LPC_GPIO2->FIOCLR1 = 0X08;
    delay(1000);
    LPC_GPIO2->FIOSET1 = 0X08;
    delay(3000); // 10MS
    LPC_GPIO2->FIOCLR1 = 0X08;

    // ----- WAIT FOR ECHO HIGH -----
    while(!(LPC_GPIO2->FIOPIN1 & 0X10));

    // ----- MEASURE ECHO HIGH TIME -----
    while(LPC_GPIO2->FIOPIN1 & 0X10)
    {
        time++;
        delay(10);
    }

    return (time / 58);   // convert to cm
}
