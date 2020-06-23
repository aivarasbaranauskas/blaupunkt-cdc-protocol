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


#define MAX_MESSAGE_LENGTH 16

#define STATUS_UNINITIALISED 0
#define STATUS_INITIALISATION_1 1
#define STATUS_INITIALISATION_2 2
#define STATUS_INITIALISATION_3 3
#define STATUS_INITIALISED 4

#define MESSAGE_START 0x180
#define MESSAGE_END 0x14F

struct Message
{
    unsigned int msg[MAX_MESSAGE_LENGTH];
    unsigned int len;
};

struct Message receive(void);
void send(struct Message m);

int main(void) {
    uint8_t status = STATUS_UNINITIALISED;
    struct Message m;

    // start at 4800
    uart_4800();

    // main loop
    while (1) {
        // receive message
        m = receive();
        
        // interpret the message
        switch (status)
        {
        case STATUS_UNINITIALISED:
            if (m.len == 3 && m.msg[1] == 0x0AD) {
                status = STATUS_INITIALISATION_1;
            }
            break;
        case STATUS_INITIALISATION_1:
            if (m.len == 4 && m.msg[1] == 0x048 && m.msg[2] == 0x001) {
                _delay_ms(30); // wait for 30ms
                m.msg[0] = 0x10F;
                m.msg[1] = 0x048;
                m.msg[2] = 0x001;
                m.msg[3] = MESSAGE_END;
                m.len = 4;
                send(m);
                status = STATUS_INITIALISATION_2;
            }
            break;
        case STATUS_INITIALISATION_2:
            if (m.len == 4 && m.msg[1] == 0x048 && m.msg[2] == 0x002) {
                uart_9600();
                status = STATUS_INITIALISATION_3;
            }
            break;
        case STATUS_INITIALISATION_3:
            // We don't case what was sent here
            status = STATUS_INITIALISED;
            break;
        case STATUS_INITIALISED:
            // TODO: send something?
            break;
        default:
            break;
        }
        
    }

    return 0;
}

struct Message receive(void) {
    unsigned int msg_received = 0;
    struct Message m;
    m.len = 0;
    while (!msg_received) {
        m.msg[m.len] = uart_receive();
        if (m.msg[m.len] == MESSAGE_END) {
            msg_received = 1;
        } else {
            // echo back everything except MESSAGE_END 
            uart_send(m.msg[m.len]);
        }
        m.len++;
    }
    return m;
}

void send(struct Message m) {
    unsigned int resp;
    for (int i = 0 ; i < m.len ; i++) {
        uart_send(m.msg[i]);
        resp = uart_receive();
        if (m.msg[i] != resp) {
            // TODO: echoed back incorrrectly
        }
    }
}