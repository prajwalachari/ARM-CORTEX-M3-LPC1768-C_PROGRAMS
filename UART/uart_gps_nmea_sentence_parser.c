/*
 * ============================================================================
 * File        : uart_gps_nmea_sentence_parser.c
 * Target      : NXP LPC1768 (ARM Cortex-M3)
 * Peripheral  : UART0, Circular Ring Buffer, NVIC
 * Description : Bare-metal UART driver interfacing GPS / GPRS module at 9600 baud.
 *               Implements interrupt-driven byte reception into a circular
 *               ring buffer, extracts NMEA $GPRMC frames delimited by
 *               newline characters, and filters location packets.
 * Pin Mapping :
 *   P0.2  --> UART0 TXD
 *   P0.3  --> UART0 RXD
 * ============================================================================
 */

#include "LPC17xx.h"
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 256

//CIRCULAR RING BUFFER STRUCTURE
typedef struct {
    uint8_t data[BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} RingBuffer;

RingBuffer gps_rx_buffer = {{0}, 0, 0};
char nmea_sentence[100]; // Global buffer to hold the final string

// --- BUFFER FUNCTIONS ---
void RingBuffer_Push(RingBuffer *buf, uint8_t val) {
    uint16_t next = (buf->head + 1) % BUFFER_SIZE;
    
    /* DEBUG HINT 1: Look closely at how the buffer checks for a full state. 
       Is it preventing an overflow, or will it accidentally lock up or drop bytes? */
    if (next == buf->tail) { 
        buf->data[buf->head] = val;
        buf->head = next;
    }
}

uint8_t RingBuffer_Pop(RingBuffer *buf, uint8_t *val) {
    if (buf->head == buf->tail) {
        return 0; // Buffer Empty
    }
    *val = buf->data[buf->tail];
    buf->tail = (buf->tail + 1) % BUFFER_SIZE;
    return 1;
}

// --- BARE-METAL UART0 INITIALIZATION ---
void UART0_Init(uint32_t baudrate) {
    uint32_t pclk, dl;

    // 1. Power up UART0 peripheral
    LPC_SC->PCONP |= (1 << 3); 

    // 2. Configure P0.2 as TXD0 and P0.3 as RXD0
    LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));
    LPC_PINCON->PINSEL0 |=  ((1 << 4) | (1 << 6)); 

    // 3. Set Peripheral Clock (PCLK_UART0) to CCLK/4
    LPC_SC->PCLKSEL0 &= ~(3 << 6); 

    // 4. Calculate Baud Rate Dividers (Assuming PCLK = 25MHz)
    pclk = 25000000;
    dl = pclk / (16 * baudrate);

    /* DEBUG HINT 2: To update the Baud Rate registers (DLL and DLM), 
       what state must the DLAB (Divisor Latch Access Bit) be in? Check the bit mask below. */
    LPC_UART0->LCR = (0 << 7) | (3 << 0); // Setting DLAB and 8-bit character length
    LPC_UART0->DLL = dl & 0xFF;
    LPC_UART0->DLM = (dl >> 8) & 0xFF;
    LPC_UART0->LCR &= ~(1 << 7);          // Disable DLAB

    // 5. Enable FIFO and Receive Data Interrupts
    LPC_UART0->FCR = 0x07;                
    LPC_UART0->IER = (1 << 0);            

    // 6. Enable UART0 Interrupt in NVIC
    NVIC_EnableIRQ(UART0_IRQn);
}

// --- UART0 INTERRUPT SERVICE ROUTINE ---
void UART0_IRQHandler(void) {
    uint32_t iir = LPC_UART0->IIR;
    
    // Check if interrupt source is Receive Data Available
    if (((iir & 0x0E) == 0x04) || ((iir & 0x0E) == 0x0C)) {
        
        /* DEBUG HINT 3: This loop is supposed to read bytes from the hardware 
           as long as the hardware RX FIFO has data. Check the Line Status Register (LSR) bit. */
        while (LPC_UART0->LSR & (0 << 0)) { 
            uint8_t incoming_byte = LPC_UART0->RBR;
            RingBuffer_Push(&gps_rx_buffer, incoming_byte);
        }
    }
}

// --- NMEA SENTENCE EXTRACTION ROUTINE ---
void GPS_GetSentence(char *out_buffer, uint16_t max_len) {
    uint16_t idx = 0;
    uint8_t ch;
    uint8_t sync_found = 0;

    while (idx < (max_len - 1)) {
        if (RingBuffer_Pop(&gps_rx_buffer, &ch)) {
            if (!sync_found) {
                if (ch == '$') { // Syncing to start of NMEA frame
                    sync_found = 1;
                    out_buffer[idx++] = ch;
                }
            } else {
                out_buffer[idx++] = ch;
                if (ch == '\n') { // Frame complete
                    break;
                }
            }
        }
    }
    out_buffer[idx] = '\0'; // Null-terminate string
}

// --- MAIN APPLICATION ENTRY ---
int main(void) {
    SystemInit();         
    UART0_Init(9600);     // SKG13 default baud rate

    while(1) {
        /* DEBUG HINT 4: Pay close attention to how the array parameter 
           is passed to this function. Is it passing the buffer correctly? */
        GPS_GetSentence(nmea_sentence[], sizeof(nmea_sentence));

        // Filter and check for the Recommended Minimum frame
        if (strstr(nmea_sentence, "RMC") != NULL) {
            // Your frame is safely stored in 'nmea_sentence' here!
            __NOP(); 
        }
    }
}
