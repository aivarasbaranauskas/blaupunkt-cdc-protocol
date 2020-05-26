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

#include "main.h"
#include "uart.h"

#define MAX_MESSAGE_LENGTH 16

#define STATUS_UNINITIALISED 0
#define STATUS_INITIALISATION_STARTED 1
#define STATUS_INITIALISED 2

#define MESSAGE_START = 0x180;
#define MESSAGE_END = 0x14F;

int main(void) {
    uint8_t status = STATUS_UNINITIALISED;
    unsigned int msg[MAX_MESSAGE_LENGTH];
    unsigned int msg_len = 0;
    bool msg_received = false;

    // start at 4800
    uart_4800();

    // main loop
    while (true) {
        // receive message
        msg_received = false;
        msg_len = 0;
        while (!msg_received) {
            msg[msg_len] = uart_receive();
            if (msg[msg_len] == MESSAGE_END) {
                msg_received = true;
            } else {
                // echo back everything except MESSAGE_END 
                uart_send(msg[msg_len]);
            }
            msg_len++;
        }
        
        // interpret the message

        
    }

    return 0;
}