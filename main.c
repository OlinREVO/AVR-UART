/* TEST CODE - UART
        Initializes the UART controller and sends out sporadic debug messages.
*/

#define F_CPU (1000000L)
#define UART_BAUD (9600L)
#define UART_SAMPLERATE (52UL)
#define BAUD_PRESCALE (((F_CPU / (UART_BAUD * UART_SAMPLERATE))) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define MAX_TX_BUFFER_SIZE 200 // maximum size of transmit buffer in bytes

// define pointers for the circular buffer
// This works by defining a buffer and writing characters to be transmitted
//  into it via the write pointer. The characters are then transmitted them
//  one at a time via the read pointer. If the write pointer passes the end
//  pointer, it loops around to the begin pointer again. If the write pointer
//  meets the read pointer, it returns with an error.
char* TX_begin;
char* TX_end;
char* TX_read;
char* TX_write;

// Set up the LIN/UART Controller
// RX on 12
// TX on 2

// Scale down to 2400 baud (bits per sec)
// 
void initUART(void) {
    // set up the transmit buffer
    TX_begin = (char*)malloc(MAX_TX_BUFFER_SIZE*sizeof(char));
    TX_end = TX_begin + MAX_TX_BUFFER_SIZE*sizeof(char);
    TX_read = TX_begin;
    TX_write = TX_begin;

    // initialize pins and interrupts
    sei(); // enable global interrupts
    DDRD &= ~(_BV(PD4)); // set pin 12 for input
    DDRD |= _BV(PD3); // set pin 2 for output

    // reset the controller and activate UART
    LINCR = _BV(SWRES) | _BV(LENA) | _BV(LCMD2) | _BV(LCMD0);
    LINBTR = 32;
    LINBRR = 12; // baud rate prescaler
    LINENIR |= _BV(LENTXOK);
}

// turn off the UART controller and free the buffer memory
void endUART(void) {
    LINCR = _BV(SWRES);
    free(TX_begin);
}

// transmission has completed successfully
// load in next byte if one exists
ISR(LIN_TC_vect) {
    if (TX_read != TX_write) {

    } else {
        LINSIR |= _BV(LTXOK);
    }
}

int UART_putChar(char c) {
    LINDAT = c;
    return(0);
    // int i = 0;
    // while((LINSIR & _BV(LBUSY)) && (i<1000) ){ ++i; } // controller is busy
    
    // if (LINSIR & _BV(LBUSY)) {
    //     LINDAT = c;
    //     return(0);
    // } else {
    //     return(-1);
    // }
}

// int UART_putString(char* s, int len) {
//     int i;
//     for (i=0;i<len;i++) {
//         // add *(s+i*8) to the circular buffer
//     }
//     return(0);
// }

int main (void) {
    DDRB |= _BV(PB7);

    initUART(); // intitialize UART controller

    for (;;) {
        // spit out debug messages forever
        UART_putChar('a');
        PORTB ^= _BV(PB7);
        _delay_ms(500);
    }

    return(0);
}

