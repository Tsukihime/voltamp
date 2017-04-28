#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "display.h"
#include "uart.h"
#include "utils.h"
#include "config.h"

Display display;

void Display::initialize() {
    SET_PORT_BIT(DISPLAY_DRECTION, DISPLAY_ENABLE_PIN);
    SET_PORT_BIT(DISPLAY_DRECTION, DISPLAY_DATA_PIN);
    SET_PORT_BIT(DISPLAY_DRECTION, DISPLAY_CLOCK_PIN);
}

static void sendUpTo16Bit(uint16_t data, uint8_t bitCount) {
    for(uint8_t i = 0; i < bitCount; i++) {
        if(data & 0x8000) { // if MSB is 1
            SET_PORT_BIT(DISPLAY_PORT, DISPLAY_DATA_PIN);
        } else {
            CLR_PORT_BIT(DISPLAY_PORT, DISPLAY_DATA_PIN);
        }
        // do clock
        SET_PORT_BIT(DISPLAY_PORT, DISPLAY_CLOCK_PIN);
        _delay_us(2);
        CLR_PORT_BIT(DISPLAY_PORT, DISPLAY_CLOCK_PIN);
        _delay_us(1); // 1us + 1 cycle loop = 2.5 us
        data <<= 1;
    }
}

static void sendDisplayBlock(TDisplay* display) {
    CLR_PORT_BIT(DISPLAY_PORT, DISPLAY_ENABLE_PIN); // ENB active LOW to start send    
    uint16_t data = 0;
    
    // send dots
    data |= display->points;
    data <<= 12;
    sendUpTo16Bit(data, 4);
    
    // send digits
    data = 0;
    data |= display->digits[0];
    data <<= 4;
    data |= display->digits[1];
    data <<= 4;
    data |= display->digits[2];
    data <<= 4;
    data |= display->digits[3];
    sendUpTo16Bit(data, 16);
    SET_PORT_BIT(DISPLAY_PORT, DISPLAY_ENABLE_PIN); // ENB HIGH to load latch
}

static void resetDisplay() {
    CLR_PORT_BIT(DISPLAY_PORT, DISPLAY_CLOCK_PIN);
    SET_PORT_BIT(DISPLAY_PORT, DISPLAY_ENABLE_PIN);
}

static void switchToNextDispay() { // Cascading    
    CLR_PORT_BIT(DISPLAY_PORT, DISPLAY_ENABLE_PIN); // ENB active LOW to start send
    sendUpTo16Bit(0xFFFF, 4); // first four is 1    
    sendUpTo16Bit(0, 16);     // remaining 16 is unimportant
    SET_PORT_BIT(DISPLAY_PORT, DISPLAY_ENABLE_PIN); // ENB HIGH to load latch
}

static bool isSameDisplays(TDisplay* first, TDisplay* second) {
    return (memcmp(first, second, sizeof(TDisplay)) == 0);
}

static void copyDisplays(TDisplay* dest, TDisplay* src) {
    memcpy(dest, src, sizeof(TDisplay));
}

void Display::sendToDisplay(TDisplay* firstDisplay, TDisplay* secondDisplay) {
    if(isSameDisplays(&oldFirstDisplay, firstDisplay) && 
       isSameDisplays(&oldSecondDisplay, secondDisplay)) {
        return;
    } 
    copyDisplays(&oldFirstDisplay, firstDisplay);
    copyDisplays(&oldSecondDisplay, secondDisplay);

    resetDisplay();
    switchToNextDispay();
    sendDisplayBlock(secondDisplay);
    sendDisplayBlock(firstDisplay);
}
