#ifndef DISPLAYFORMAT_H_
#define DISPLAYFORMAT_H_

#include <stdint.h>
#include "display.h"

class Format {
    public:
    static void fixedPointWithMinusSign(int16_t value, TDisplay* display);
    static void floatingPoint4Didgits(uint16_t value, TDisplay* display);
    static void fixedPoint4Didgits(uint16_t value, TDisplay* display);
    static void fixedPointWithLeadingBlank(uint16_t value, TDisplay* display);    
};


#endif /* DISPLAYFORMAT_H_ */
