/*
 * ============================================================================
 * File        : can_ultrasonic_distance_tx.c
 * Target      : NXP LPC1768 (ARM Cortex-M3) - CAN Node 22
 * Peripheral  : CAN1, Timer0, GPIO
 * Description : Ultrasonic obstacle detector with CAN bus transmitter.
 *               Uses HC-SR04 sensor timed via Timer0 (1 us tick).
 *               If obstacle is detected within 3 meters (<300 cm), transmits
 *               alert char '1' to Node 21 (ID: 0x21); otherwise transmits '0'.
 * Pin Mapping :
 *   P0.0  --> CAN1 RD
 *   P0.1  --> CAN1 TD
 *   P0.21 --> Ultrasonic TRIG (Output Pulse)
 *   P0.22 --> Ultrasonic ECHO (Input Pulse)
 * ============================================================================
 */

/* CAN Node 22 - Ultrasonic Sensor Transmitter
 * Connections:
 *   TRIG -> P0.21 (Output)
 *   ECHO -> P0.22 (Input)
 *
 * Logic:
 *   Obstacle < 3m (300 cm)  -> Sends '1' to Node 21 (ID: 0x00000021)
 *   Obstacle >= 3m (away)   -> Sends '0' to Node 21 (ID: 0x00000021)
 */

#include <LPC17xx.h>

void CAN_Init(void);
void CAN_ACC(void);
void looktable(void);
void can_tx(unsigned long int id, unsigned char msg);

void Timer0_Init(void);
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);
unsigned int get_distance_cm(void);

// Pin Definitions
#define TRIG_PIN (1 << 21)  // P0.21 as TRIG (Output)
#define ECHO_PIN (1 << 22)  // P0.22 as ECHO (Input)

int main()
{
    unsigned char obstacle_state = 0;
    unsigned char prev_state = 0xFF; // Force initial transmission
    unsigned int distance = 0;

    SystemInit();

    // 1. Configure P0.21 (TRIG) and P0.22 (ECHO) as GPIO
    LPC_PINCON->PINSEL1 &= ~((3 << 10) | (3 << 12)); 

    // 2. Set Directions: P0.21 = OUTPUT, P0.22 = INPUT
    LPC_GPIO0->FIODIR |= TRIG_PIN;   // P0.21 Output
    LPC_GPIO0->FIODIR &= ~ECHO_PIN;  // P0.22 Input
    LPC_GPIO0->FIOCLR  = TRIG_PIN;   // Ensure TRIG starts LOW

    // 3. Initialize Peripherals
    Timer0_Init();
    CAN_Init();
    CAN_ACC();
    looktable();

    while (1)
    {
        distance = get_distance_cm();

        // 3 meters = 300 cm
        if (distance > 2 && distance < 300) {
            obstacle_state = '1';
        } else {
            obstacle_state = '0';
        }

        // Send over CAN only when obstacle state changes
        if (obstacle_state != prev_state)
        {
            prev_state = obstacle_state;
            can_tx(0x00000021, obstacle_state);
        }

        delay_ms(100); // 100ms cycle between pulses
    }
}

/* Timer0 initialized for 1 microsecond per tick (assuming 100MHz / 4 = 25MHz PCLK) */
void Timer0_Init(void)
{
    LPC_SC->PCONP |= (1 << 1);     // Power up Timer0
    LPC_SC->PCLKSEL0 &= ~(3 << 2); // PCLK = CCLK / 4
    LPC_TIM0->CTCR = 0x0;          // Timer mode
    LPC_TIM0->PR   = 24;           // 25 - 1 = 24 (1 tick = 1 us)
    LPC_TIM0->TCR  = 0x02;         // Reset Timer
}

void delay_us(unsigned int us)
{
    LPC_TIM0->TCR = 0x02; // Reset
    LPC_TIM0->TCR = 0x01; // Enable
    while (LPC_TIM0->TC < us);
    LPC_TIM0->TCR = 0x00; // Disable
}

void delay_ms(unsigned int ms)
{
    delay_us(ms * 1000);
}

/* Measure obstacle distance in cm using integer arithmetic */
unsigned int get_distance_cm(void)
{
    unsigned int width = 0;
    unsigned int timeout = 0;

    // Send 10us Trigger pulse on P0.21
    LPC_GPIO0->FIOCLR = TRIG_PIN;
    delay_us(2);
    LPC_GPIO0->FIOSET = TRIG_PIN;
    delay_us(10);
    LPC_GPIO0->FIOCLR = TRIG_PIN;

    // Wait for ECHO (P0.22) to go HIGH
    timeout = 100000;
    while (!(LPC_GPIO0->FIOPIN & ECHO_PIN)) {
        if (--timeout == 0) return 400; // Timeout, return out-of-range (>3m)
    }

    // Start Timer0 to measure duration
    LPC_TIM0->TCR = 0x02; // Reset
    LPC_TIM0->TCR = 0x01; // Enable

    // Wait for ECHO (P0.22) to go LOW
    while (LPC_GPIO0->FIOPIN & ECHO_PIN) {
        if (LPC_TIM0->TC > 35000) break; // Timeout after 35ms (~6 meters)
    }
    width = LPC_TIM0->TC;
    LPC_TIM0->TCR = 0x00; // Stop timer

    // Distance in cm = width_in_us / 58
    if (width == 0) return 400;
    return (width / 58);
}

void CAN_Init(void)
{
    LPC_SC->PCONP |= 0x00002000;
    LPC_SC->PCLKSEL0 |= 0x00000000;
    LPC_PINCON->PINSEL0 |= 0x00000005;
    LPC_CAN1->MOD = 0x00000001;
    LPC_CAN1->CMR = 0x00000000;
    LPC_CAN1->GSR = 0x00000000;
    LPC_CAN1->IER = 0x00000001;
    LPC_CAN1->BTR = 0x001C0007;
    LPC_CAN1->MOD = 0x00000000;
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

void can_tx(unsigned long int id, unsigned char msg)
{
    while ((LPC_CAN1->SR & 0x00000004) != 0x00000004);
    LPC_CAN1->TFI1 = 0x80010000;
    LPC_CAN1->TID1 = id;
    LPC_CAN1->TDA1 = msg;
    LPC_CAN1->CMR  = 0x21;
}
