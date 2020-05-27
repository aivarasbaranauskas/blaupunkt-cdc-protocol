/*
 * Demonstration on how to redirect stdio to UART. 
 *
 * http://appelsiini.net/2011/simple-usart-with-avr-libc
 *
 * To compile and upload run: make clean; make; make program;
 * Connect to serial with: screen /dev/tty.usbserial-*
 *
 * Copyright 2011 Mika Tuupola
 *
 * Licensed under the MIT license:
 *   http://www.opensource.org/licenses/mit-license.php
 *
 */
 
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

int main(void) {
    unsigned int r;
    uart_4800();

    for (;;) {
        r = uart_receive();
        _delay_ms(10);
        uart_send(r);
    }

    return 0;
}