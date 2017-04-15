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
    if(currentChar >= sizeof(TUARTSTR)) {
        currentChar = 0;
        CLR_PORT_BIT(UCSRB, TXCIE); // Disable the USART Transmit Complete interrupt (USART_TXC)
        return;
    }

    if(currentChar == 0) {
        SET_PORT_BIT(UCSRB, TXCIE); // Enable the USART Transmit Complete interrupt (USART_TXC)
    }

    UDR = uartData.chars[currentChar++];
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
    uartData.data.a = 'A';
    uartData.data.v = 'V';
    uartData.data.c = 'C';
    uartData.data.space = ' ';
    uartData.data.space2 = ' ';
    uartData.data.nline = 13; // \n
    
    currentChar = 0;
}

void Uart::updateData(uint16_t millivolts, uint16_t milliamperes, int16_t degrees) {
    uint8_t buffer[4];

    // volts
    if(millivolts > 9999) {
        bin2bcd(millivolts / 10, buffer);
        bcd2text(buffer);
        uartData.data.volts[0] = buffer[0];
        uartData.data.volts[1] = buffer[1];
        uartData.data.volts[2] = '.';
        uartData.data.volts[3] = buffer[2];
        uartData.data.volts[4] = buffer[3];
        
        } else {
        bin2bcd(millivolts, buffer);
        bcd2text(buffer);
        uartData.data.volts[0] = buffer[0];
        uartData.data.volts[1] = '.';
        uartData.data.volts[2] = buffer[1];
        uartData.data.volts[3] = buffer[2];
        uartData.data.volts[4] = buffer[3];
    }

    // amps
    bin2bcd(milliamperes, buffer);
    bcd2text(buffer);
    uartData.data.amperes[0] = buffer[0];
    uartData.data.amperes[1] = '.';
    uartData.data.amperes[2] = buffer[1];
    uartData.data.amperes[3] = buffer[2];
    uartData.data.amperes[4] = buffer[3];
    
    // degrees
    bool needMinusSign = false;
    if(degrees < 0) {
        degrees = -degrees;
        needMinusSign = true;
    }

    bin2bcd(degrees, buffer);
    bcd2text(buffer);
    
    uartData.data.temperature[0] = buffer[0];
    uartData.data.temperature[1] = buffer[1];
    uartData.data.temperature[2] = buffer[2];
    uartData.data.temperature[3] = '.';
    uartData.data.temperature[4] = buffer[3];
    
    int i = 0;  // hide leading zeros
    while(uartData.data.temperature[i] == '0' && i < 2) {
        uartData.data.temperature[i] = ' ';
        i++;
    }
    
    if(needMinusSign) {
        uartData.data.temperature[--i] = '-';
    }

    sendUartData();
}
