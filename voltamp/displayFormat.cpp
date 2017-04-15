#include "displayFormat.h"
#include "utils.h"

void Format::degrees(int16_t degrees, TDisplay* display) { // [ -1.5] [ 22.1] [120.6]
    display->points = DISPLAY_THIRD_DOT;
    
    bool needMinusSign = false;
    if(degrees < 0) {
        degrees = -degrees;
        needMinusSign = true;
    }
    bin2bcd(degrees, display->digits);
    
    // remove leading zeros
    int i = 0;
    while(display->digits[i] == 0 && i < 2){
        display->digits[i] = DSPLAY_BLANK_CHAR;
        i++;
    }
    
    if(needMinusSign) {
        display->digits[--i] = DSPLAY_DASH_CHAR;
    }
}

void Format::watts(uint16_t milliwatts, TDisplay* display) { // [17.52] [2.592]
    volts(milliwatts, display);
}

void Format::amperes(uint16_t milliamperes, TDisplay* display) { // [2.004] [0.100]
    bin2bcd(milliamperes, display->digits);
    display->points = DISPLAY_FIRST_DOT;
}

void Format::volts(uint16_t millivolts, TDisplay* display) { // [10.00] [0.054]
    if(millivolts > 9999) {
        display->points = DISPLAY_SECOND_DOT;
        bin2bcd(millivolts / 10, display->digits);
    } else {
        display->points = DISPLAY_FIRST_DOT;
        bin2bcd(millivolts, display->digits);
    }
}
