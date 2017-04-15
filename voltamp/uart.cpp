#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "utils.h"
#include "config.h"

Uart uart;

#ifdef UART_DOUBLESPEED
    #define UART_CALC_BAUDRATE(baud) ((uint32_t)F_CPU / ((uint32_t)baud * 8) - 1)
#else
    #define UART_CALC_BAUDRATE(baud) ((uint32_t)F_CPU / ((uint32_t)baud * 16) - 1)
#endif

ISR(USART_TXC_vect) {
    uart.sendUartData();
}

void Uart::sendUartData() {
    switch (currentChar) {
        case 0:
            SET_PORT_BIT(UCSRB, TXCIE); // Enable the USART Transmit Complete interrupt (USART_TXC)
            // not break do case 1 too
        case 1 ... (sizeof(TUARTSTR) - 1):
            UDR = uartData.chars[currentChar++];
            break;

        default:
            currentChar = 0;
            CLR_PORT_BIT(UCSRB, TXCIE); // Disable the USART Transmit Complete interrupt (USART_TXC)
    }
}

void Uart::Initialize(void) {
    // Set baud rate
    UBRRH = (UART_CALC_BAUDRATE(UART_BAUDRATE) >> 8) & 0xFF;
    UBRRL = (UART_CALC_BAUDRATE(UART_BAUDRATE) & 0xFF);

    UCSRB = (1 << TXEN); // Turn on the transmission circuitry

    UCSRC = (1 << URSEL) |               // select UCSRC register
            (0 << UMSEL) |               // Asynchronous Operation
            (0 << UPM1) | (0 << UPM0) |  // no parity
            (0 << USBS) |                // 1stop bit
            (1 << UCSZ1) | (1 << UCSZ0); // 8 bit data

#ifdef UART_DOUBLESPEED
    UCSRA = (1 << U2X);
#else
    UCSRA = 0;
#endif
    // init uart str
    uartData.a = 'A';
    uartData.v = 'V';
    uartData.c = 'C';
    uartData.space = ' ';
    uartData.space2 = ' ';
    uartData.nline = 13; // \n
    
    currentChar = 0;
}

void Uart::updateData(uint16_t millivolts, uint16_t milliamperes, int16_t degrees) {
    uint8_t buffer[4];

// volts
    if(millivolts > 9999) {
        bin2bcd(millivolts / 10, buffer);
        bcd2text(buffer);
        uartData.volts[0] = buffer[0];
        uartData.volts[1] = buffer[1];
        uartData.volts[2] = '.';
        uartData.volts[3] = buffer[2];
        uartData.volts[4] = buffer[3];
        
    } else {
        bin2bcd(millivolts, buffer);
        bcd2text(buffer);
        uartData.volts[0] = buffer[0];
        uartData.volts[1] = '.';
        uartData.volts[2] = buffer[1];
        uartData.volts[3] = buffer[2];
        uartData.volts[4] = buffer[3];
    }
// amps
    bin2bcd(milliamperes, buffer);
    bcd2text(buffer);
    uartData.amperes[0] = buffer[0];
    uartData.amperes[1] = '.';
    uartData.amperes[2] = buffer[1];
    uartData.amperes[3] = buffer[2];
    uartData.amperes[4] = buffer[3];
    
// degrees    
    bool needMinusSign = false;
    if(degrees < 0) {
        degrees = -degrees;
        needMinusSign = true;        
    }

    bin2bcd(degrees, buffer);    
    bcd2text(buffer);
    
    uartData.temperature[0] = buffer[0];
    uartData.temperature[1] = buffer[1];    
    uartData.temperature[2] = buffer[2];
    uartData.temperature[3] = '.';
    uartData.temperature[4] = buffer[3];
    
    int i = 0;  // hide leading zeros
    while(uartData.temperature[i] == '0' && i < 2) {
        uartData.temperature[i] = ' ';
        i++;        
    }
    
    if(needMinusSign) {
        uartData.temperature[--i] = '-';
    }

    sendUartData();
}
