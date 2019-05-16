#include <avr/io.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

void uart_9600(void) {
#undef BAUD
#define BAUD 9600
#include <util/setbaud.h>

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    uart_shared();
}

void uart_4800(void) {
#undef BAUD
#define BAUD 4800
#include <util/setbaud.h>

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    uart_shared();
}

void uart_shared(void) {
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00) | _BV(UCSZ20); /* 9-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_send(unsigned int data) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    /* Copy 9th bit to TXB8 */
    UCSR0B &= ~(1<<TXB8);
    if ( data & 0x0100 ) {
        UCSR0B |= (1<<TXB8);
    }
    UDR0 = data;
}

unsigned int uart_receive() {
    unsigned char status, resh, resl;
    /* Wait for data to be received */
    loop_until_bit_is_set(UCSR0A, RXC0);
    /* Get status and 9th bit, then data from buffer */
    status = UCSR0A;
    resh = UCSR0B;
    resl = UDR0;
    /* If error, return -1 */
    if ( status & (1<<FE0)|(1<<DOR0)|(1<<UPE0) ) {
        return -1;
    }
    /* Filter the 9th bit, then return */
    resh = (resh >> 1) & 0x01;
    return ((resh << 8) | resl);
}