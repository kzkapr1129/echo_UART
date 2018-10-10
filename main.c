// INCLUDE
#include <xc.h>
#include <stdio.h>

// CONFIG
#pragma config FOSC = INTOSC, WDTE = OFF, LVP = OFF

// DEFILE
#define _XTAL_FREQ 8000000

void sendUSART(unsigned char c) {
    while (!TXSTAbits.TRMT); // wait for TX is ready
    TXREG = c;
}

unsigned char reciveUSART() {
    unsigned char c = 0;
    
    while (PIR1bits.RCIF == 0); // wait for USART is received

    // check whether error occur
    if ((RCSTAbits.OERR) || (RCSTAbits.FERR)) {
        // the overrun error or the framing error occurred
        
        // restart USART
        RCSTA = 0;    // disable USART
        RCSTA = 0x90; // enable USART

    } else {
        c = RCREG; // get byte received by USART
    }
    
    return c;
}

void main(void) {    
    /* set a clock frequency */
    OSCCON = 0b01110010; // 8MHz
    
    /* set port status */
    ANSELA  = 0b00000000; // all of pins are digital
    ANSELB  = 0b00000000; // all of pins are digital
    TRISA   = 0b00000000; // all of pins are output
    TRISB   = 0b00000110; // RB1(RX) and RB2(TX) are input
    PORTA   = 0b00000000; // initialize portA
    PORTB   = 0b00000000; // initialize portB

    /* set USART settings */
    TXSTA   = 0x24; // TXEN(TX enable), BRGH(high speed)
    RCSTA   = 0x90; // SPEN(serial port enable), CREN(continually receive USART)
    BAUDCON = 0x08; // BRG16 (make 115.2 BAUD RATE is available)
    SPBRG   = 16; // actual rate: 115200 (data seat p303))
    RXDTSEL = 0; // RB1 to RX (data seat p.118)
    TXCKSEL = 0; // RB2 to TX (data seat p.118)

    /* set interruption settings */
    PIR1bits.RCIF   = 0; // clear interrupt flag
    PIE1bits.RCIE   = 1; // allow interruption of USART
    INTCONbits.PEIE = 1; // allow interrupting peripheral
    INTCONbits.GIE  = 1; // allow global

    while (1) {
        unsigned char c = reciveUSART();
        if (c == 'a') {
            sendUSART('*');
        } else {
            sendUSART('-');
        }
    }
}
