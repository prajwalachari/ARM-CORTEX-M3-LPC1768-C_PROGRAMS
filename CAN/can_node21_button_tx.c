/*
 * File    : can_node21_button_tx.c
 * Target  : LPC1768 (ARM Cortex-M3) - CAN Node ID 0x21 (Transmitter)
 * Purpose : CAN Bus Transmitter Node 21.
 *           Reads a push button on P0.21 and transmits a CAN message
 *           to Node 22 (ID 0x00000022) whenever the button state changes.
 *           Sends character '0' when button pressed, '1' when released.
 *           Uses interrupt-driven CAN reception for incoming messages and
 *           drives an LED output based on what Node 22 sends back.
 *
 * Pin Map :
 *   P0.0  --> CAN1 RD (Receive Data)
 *   P0.1  --> CAN1 TD (Transmit Data)
 *   P0.21 --> Push Button Input
 *   P1.29 --> On-board LED Output (controlled by received CAN data)
 *
 * CAN Config:
 *   Baud Rate : ~500 kbps (BTR = 0x001C0007 @ 25 MHz PCLK)
 *   Node ID   : 0x21 (this node transmits)
 *   Target ID : 0x22 (remote node receives)
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

    /* Pull-down on P0.21 (button) */
    LPC_PINCON->PINMODE1 |= (3 << 10);

    /* P1.29 as LED output */
    LPC_GPIO1->FIOMASK3 = 0xDF;
    LPC_GPIO1->FIODIR3  = 0x20;

    /* P0.21 as INPUT */
    LPC_GPIO0->FIOMASK = 0xFFDFFFFF;
    LPC_GPIO0->FIODIR  = 0xFFDFFFFF;

    CAN_Init();
    CAN_ACC();
    looktable();

    while(1)
    {
        rx = (LPC_GPIO0->FIOPIN >> 21) & 0x01;

        if(rx != prev)
        {
            prev = rx;
            /* Transmit button state to Node 22 */
            if(rx == 0)
                can_tx(0x00000021, '0');
            else
                can_tx(0x00000021, '1');
        }
    }
}

void CAN_Init(void)
{
    LPC_SC->PCONP       |= 0x00002000;
    LPC_SC->PCLKSEL0    |= 0x00000000;
    LPC_PINCON->PINSEL0 |= 0x00000005;
    LPC_CAN1->MOD = 0x00000001;
    LPC_CAN1->CMR = 0x00000000;
    LPC_CAN1->GSR = 0x00000000;
    LPC_CAN1->IER = 0x00000001;
    LPC_CAN1->BTR = 0x001C0007;
    LPC_CAN1->MOD = 0x00000000;
    NVIC_EnableIRQ(CAN_IRQn);
}

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

void looktable(void)
{
    LPC_CANAF->AFMR        = 0x00000001;
    LPC_CANAF_RAM->mask[0] = 0x00000020;
    LPC_CANAF_RAM->mask[1] = 0x00000021;
    LPC_CANAF_RAM->mask[2] = 0x00000022;
    LPC_CANAF->AFMR        = 0x00000000;
}

void delay(unsigned long int z)
{
    unsigned long int x;
    for(x = 0; x < z; x++);
}

void can_tx(unsigned long int id, unsigned char msg)
{
    while((LPC_CAN1->SR & 0x00000004) != 0x00000004);
    LPC_CAN1->TFI1 = 0x80010001; /* Standard frame, DLC=1, priority=1 */
    LPC_CAN1->TID1 = id;
    LPC_CAN1->TDA1 = msg;
    LPC_CAN1->CMR  = 0x21;
}

void CAN_IRQHandler(void)
{
    can_rx();
}

void can_rx(void)
{
    unsigned long int CA = 0, CB = 0, CC = 0, CD = 0;

    CA = LPC_CAN1->RFS & 0x20000000;
    if(CA == 0x00000000)
    {
        CB = LPC_CAN1->RID & 0x000007FF;
        if(CB == 0x00000022) /* Message from Node 22 */
        {
            CC = LPC_CAN1->RFS & 0x000F0000;
            if(CC == 0x00010000)
            {
                CD = LPC_CAN1->RDA & 0x000000FF;
                if(CD == '1')
                    LPC_GPIO1->FIOSET3 = 0x20;
                else
                    LPC_GPIO1->FIOCLR3 = 0x20;
            }
        }
        LPC_CAN1->CMR = 0x04;
    }
}
