#ifndef DISPLAYFORMAT_H_
#define DISPLAYFORMAT_H_

#include <stdint.h>
#include "display.h"

class Format {
    public:
    static void degrees(int16_t degrees, TDisplay* display);
    static void watts(uint16_t milliwatts, TDisplay* display);
    static void amperes(uint16_t milliamperes, TDisplay* display);
    static void volts(uint16_t millivolts, TDisplay* display);    
};


#endif /* DISPLAYFORMAT_H_ */
