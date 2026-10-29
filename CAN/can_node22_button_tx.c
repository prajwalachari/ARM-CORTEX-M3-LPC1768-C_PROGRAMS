/*
 * File    : can_node22_button_tx.c
 * Target  : LPC1768 (ARM Cortex-M3) - CAN Node ID 0x22 (Transmitter)
 * Purpose : CAN Bus Transmitter Node 22.
 *           Reads a push button on P0.21 and transmits a CAN message
 *           to Node 21 (ID 0x00000021) whenever the button state changes.
 *           Sends character '1' when button pressed, '0' when released.
 *           Uses interrupt-driven CAN reception for any incoming messages.
 *
 * Pin Map :
 *   P0.0  --> CAN1 RD (Receive Data)
 *   P0.1  --> CAN1 TD (Transmit Data)
 *   P0.21 --> Push Button Input
 *   P1.29 --> On-board LED Output (controlled by received CAN data)
 *
 * CAN Config:
 *   Baud Rate : ~500 kbps (BTR = 0x001C0007 @ 25 MHz PCLK)
 *   Node ID   : 0x22 (this node transmits)
 *   Target ID : 0x21 (remote node receives)
 */

#include <LPC17xx.h>

void CAN_Init(void);
void CAN_ACC(void);
void looktable(void);
void delay(unsigned long int z);
void can_tx(unsigned long int id, unsigned char msg);
void can_rx(void);

unsigned char rx = 0, prev = 0;

int main()
{
    SystemInit();

    /* Pull-down mode on P0.21 (button) */
    LPC_PINCON->PINMODE1 |= (3 << 10);

    /* P1.29 as LED output */
    LPC_GPIO1->FIOMASK3 = 0xDF;
    LPC_GPIO1->FIODIR3  = 0x20;
    LPC_GPIO1->FIOSET3  = 0x20; /* LED initially ON */

    /* P0.21 as INPUT (button) */
    LPC_GPIO0->FIOMASK = 0xFFDFFFFF;
    LPC_GPIO0->FIODIR  = 0xFFDFFFFF;

    CAN_Init();
    CAN_ACC();
    looktable();

    while(1)
    {
        rx = (LPC_GPIO0->FIOPIN >> 21) & 0x01;

        if(prev != rx)
        {
            prev = rx;
            /* Transmit button state to Node 21 */
            if(rx == 1)
                can_tx(0x00000022, '1');
            else
                can_tx(0x00000022, '0');
        }
    }
}

/* Initialize CAN1 peripheral */
void CAN_Init(void)
{
    LPC_SC->PCONP     |= 0x00002000;     /* Power CAN1 */
    LPC_SC->PCLKSEL0  |= 0x00000000;     /* PCLK = CCLK/4 */
    LPC_PINCON->PINSEL0 |= 0x00000005;   /* P0.0 = RD1, P0.1 = TD1 */
    LPC_CAN1->MOD = 0x00000001;          /* Reset mode */
    LPC_CAN1->CMR = 0x00000000;
    LPC_CAN1->GSR = 0x00000000;
    LPC_CAN1->IER = 0x00000001;          /* Enable RX interrupt */
    LPC_CAN1->BTR = 0x001C0007;          /* Bit timing ~500kbps */
    LPC_CAN1->MOD = 0x00000000;          /* Normal operating mode */
    NVIC_EnableIRQ(CAN_IRQn);
}

/* Configure Acceptance Filter */
void CAN_ACC(void)
{
    LPC_CANAF->AFMR       = 0x00000001;
    LPC_CANAF->SFF_sa     = 0x00000000;
    LPC_CANAF->EFF_sa     = 0x00000000;
    LPC_CANAF->SFF_GRP_sa = 0x00000000;
    LPC_CANAF->EFF_GRP_sa = 0x0000000C;
    LPC_CANAF->ENDofTable = 0x0000000C;
    LPC_CANAF->AFMR       = 0x00000000;
}

/* Load Acceptance Filter lookup table with allowed CAN IDs */
void looktable(void)
{
    LPC_CANAF->AFMR          = 0x00000001;
    LPC_CANAF_RAM->mask[0]   = 0x00000020;
    LPC_CANAF_RAM->mask[1]   = 0x00000021;
    LPC_CANAF_RAM->mask[2]   = 0x00000022;
    LPC_CANAF->AFMR          = 0x00000000;
}

void delay(unsigned long int z)
{
    unsigned long int x;
    for(x = 0; x < z; x++);
}

/* Transmit one byte over CAN using Transmit Buffer 1 */
void can_tx(unsigned long int id, unsigned char msg)
{
    while((LPC_CAN1->SR & 0x00000004) != 0x00000004); /* Wait for TX ready */
    LPC_CAN1->TFI1 = 0x80010000; /* Standard frame, DLC=1 */
    LPC_CAN1->TID1 = id;
    LPC_CAN1->TDA1 = msg;
    LPC_CAN1->CMR  = 0x21;       /* Select TX buffer 1 + Send Now */
}

void CAN_IRQHandler(void)
{
    can_rx();
}

/* Receive CAN message and drive LED on P1.29 */
void can_rx(void)
{
    unsigned long int CA = 0, CB = 0, CC = 0, CD = 0;

    CA = LPC_CAN1->RFS & 0x20000000; /* Check frame type (standard/extended) */
    if(CA == 0x00000000)              /* Standard frame (11-bit ID) */
    {
        CB = LPC_CAN1->RID & 0x000007FF; /* Read received ID */
        if(CB == 0x00000021)              /* Message from Node 21 */
        {
            CC = LPC_CAN1->RFS & 0x000F0000; /* Check DLC */
            if(CC == 0x00010000)             /* DLC = 1 byte */
            {
                CD = LPC_CAN1->RDA & 0x000000FF; /* Read data byte */
                if(CD == '1')
                    LPC_GPIO1->FIOSET3 = 0x20; /* LED ON */
                else
                    LPC_GPIO1->FIOCLR3 = 0x20; /* LED OFF */
            }
        }
        LPC_CAN1->CMR = 0x04; /* Release receive buffer */
    }
}
