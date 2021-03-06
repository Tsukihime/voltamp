#ifndef UART_H
#define UART_H

extern "C" void USART_TXC_vect(void) __attribute__((signal));

class Uart {        
    friend void USART_TXC_vect();

    private:    
        typedef union {
            struct {
                char volts[5]; // 14.12, 2.365
                char v;        // V
                char space;    // _
                char amperes[5];  // 1.541 0.312
                char a;        // A
                char space2;   // _
                char temperature[5];  // -10.1 02.3
                char c;        // C
                char nline;    // \n
            } data;
            char chars[21];
        } TUARTSTR __attribute__ ((aligned (1)));

        TUARTSTR uartData;        
        volatile uint8_t currentChar;

        void startDataTransmit();
        void ISRTransmitComplete();

    public:
        void initialize(void);
        void updateData(uint16_t millivolts, uint16_t milliamperes, int16_t degrees);
};

extern Uart uart;

#endif /* UART_H */
