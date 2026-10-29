/*
 * ============================================================================
 * File        : can_ultrasonic_alert_rx.c
 * Target      : NXP LPC1768 (ARM Cortex-M3) - CAN Node 21
 * Peripheral  : CAN1 Controller & GPIO
 * Description : CAN receiver for ultrasonic obstacle warning.
 *               Receives CAN frames on ID 0x21.
 *               If data '1' (obstacle < 3m) received -> blinks LED on P1.29.
 *               If data '0' (obstacle clear) received -> turns LED OFF.
 * Pin Mapping :
 *   P0.0  --> CAN1 RD
 *   P0.1  --> CAN1 TD
 *   P1.29 --> On-board Alert LED (Output)
 * ============================================================================
 */

/* CAN Node 21 - Receiver
 * Output: Built-in LED on P1.29
 *
 * Behavior:
 *   If '1' received (obstacle < 3m)  -> Built-in LED BLINKS continuously
 *   If '0' received (obstacle >= 3m) -> Built-in LED turns and stays OFF
 */

#include <LPC17xx.h>

void CAN_Init(void);
void CAN_ACC(void);
void looktable(void);
void can_rx(void);
void delay(unsigned long int z);

// 1 = Obstacle near (< 3m), 0 = Obstacle away (>= 3m)
volatile unsigned char obstacle_alert = 0;

int main()
{
    SystemInit();

    // Configure Built-in LED on P1.29 (Byte 3, Bit 5: 0x20)
    LPC_GPIO1->FIOMASK3 = 0xDF;  // Unmask bit 5 of Byte 3 (P1.29)
    LPC_GPIO1->FIODIR3  = 0x20;  // Set P1.29 as OUTPUT
    LPC_GPIO1->FIOCLR3  = 0x20;  // Initial state: OFF

    CAN_Init();
    CAN_ACC();
    looktable();

    while (1)
    {
        if (obstacle_alert == 1)
        {
            // Blink Built-in LED repeatedly while obstacle is within 3m
            LPC_GPIO1->FIOSET3 = 0x20; // LED ON
            delay(500000);
            LPC_GPIO1->FIOCLR3 = 0x20; // LED OFF
            delay(500000);
        }
        else
        {
            // Keep Built-in LED OFF when obstacle is away
            LPC_GPIO1->FIOCLR3 = 0x20;
        }
    }
}

void CAN_Init(void)
{
    LPC_SC->PCONP |= 0x00002000;       // CAN1 Power
    LPC_SC->PCLKSEL0 |= 0x00000000;
    LPC_PINCON->PINSEL0 |= 0x00000005; // P0.0 = RD1, P0.1 = TD1
    LPC_CAN1->MOD = 0x00000001;        // Reset mode
    LPC_CAN1->CMR = 0x00000000;
    LPC_CAN1->GSR = 0x00000000;
    LPC_CAN1->IER = 0x00000001;        // Enable RX interrupt
    LPC_CAN1->BTR = 0x001C0007;        // Baud rate configuration
    LPC_CAN1->MOD = 0x00000000;        // Normal operating mode
    NVIC_EnableIRQ(CAN_IRQn);
}

void CAN_ACC(void)
{
    LPC_CANAF->AFMR = 0x00000001;
    LPC_CANAF->SFF_sa = 0x00000000;
    LPC_CANAF->EFF_sa = 0x00000000;
    LPC_CANAF->SFF_GRP_sa = 0x00000000;
    LPC_CANAF->EFF_GRP_sa = 0x0000000C;
    LPC_CANAF->ENDofTable = 0x0000000C;
    LPC_CANAF->AFMR = 0x00000000;
}

void looktable(void)
{
    LPC_CANAF->AFMR = 0x00000001;
    LPC_CANAF_RAM->mask[0] = 0x00000020;
    LPC_CANAF_RAM->mask[1] = 0x00000021;
    LPC_CANAF_RAM->mask[2] = 0x00000022;
    LPC_CANAF->AFMR = 0x00000000;
}

void delay(unsigned long int z)
{
    unsigned long int x;
    for (x = 0; x < z; x++);
}

void CAN_IRQHandler(void)
{
    can_rx();
}

void can_rx(void)
{
    unsigned long int CB = 0, CC = 0, CD = 0;

    // Read received CAN Identifier
    CB = LPC_CAN1->RID & 0x1FFFFFFF;

    // Check if message is for ID 0x00000021
    if (CB == 0x00000021)
    {
        CC = LPC_CAN1->RFS & 0x000F0000; // Check DLC (DLC = 1 -> 0x00010000)
        if (CC == 0x00010000)
        {
            CD = LPC_CAN1->RDA & 0x000000FF; // Read received data byte

            if (CD == '1') {
                obstacle_alert = 1; // Obstacle detected (< 3m) -> start blinking LED
            } else {
                obstacle_alert = 0; // Obstacle away (>= 3m) -> stop blinking
                LPC_GPIO1->FIOCLR3 = 0x20; // Turn LED OFF immediately
            }
        }
    }
    LPC_CAN1->CMR = 0x04; // Release receive buffer
}
