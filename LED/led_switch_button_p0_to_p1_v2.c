/*
 * ============================================================================
 * File        : led_switch_button_p0_to_p1_v2.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : GPIO Port 0 & Port 1
 * Description : Digital input monitoring using GPIO pin masking.
 *               Reads P0.0 input state and drives on-board LED on P1.29.
 * Pin Mapping :
 *   P0.0  --> Digital Input (Switch / Button)
 *   P1.29 --> LED Output
 * ============================================================================
 */

#include <LPC17xx.h>

int main(void) {
    LPC_SC->PCONP |= 0x0;

  
    LPC_GPIO1->FIOMASK &= ~(1 << 29); 
    LPC_GPIO1->FIODIR |= (1 << 29);   

    LPC_GPIO0->FIOMASK &= ~(1 << 0);  
    LPC_GPIO0->FIODIR &= ~(1 << 0);  

    while (1) {
       
        if (LPC_GPIO0->FIOPIN & (1 << 0)) {
          
            LPC_GPIO1->FIOSET = (1 << 29);
        } else {
           
            LPC_GPIO1->FIOCLR = (1 << 29);
        }
    }
}
