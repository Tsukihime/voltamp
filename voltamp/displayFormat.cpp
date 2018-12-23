#include "displayFormat.h"
#include "utils.h"

void Format::fixedPointWithMinusSign(int16_t value, TDisplay* display) { // [ -1.5] [ 22.1] [120.6]
    display->points = DISPLAY_THIRD_DOT;
    
    bool needMinusSign = false;
    if(value < 0) {
        value = -value;
        needMinusSign = true;
    }
    bin2bcd(value, display->digits);
    
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

void Format::floatingPoint4Didgits(uint16_t value, TDisplay* display) { // [17.52] [2.592]
    if(value > 9999) {
        display->points = DISPLAY_SECOND_DOT;
        bin2bcd(value / 10, display->digits);
        } else {
        display->points = DISPLAY_FIRST_DOT;
        bin2bcd(value, display->digits);
    }
}

void Format::fixedPoint4Didgits(uint16_t value, TDisplay* display) { // [2.004] [0.100]
    bin2bcd(value, display->digits);
    display->points = DISPLAY_FIRST_DOT;
}

void Format::fixedPointWithLeadingBlank(uint16_t value, TDisplay* display) { // [10.00] [ 0.54]
    display->points = DISPLAY_SECOND_DOT;
    bin2bcd((value + 5) / 10, display->digits);
    if(value < 10000) {
        display->digits[0] = DSPLAY_BLANK_CHAR;
    }
}
