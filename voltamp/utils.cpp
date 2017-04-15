#include <stdint.h>

void bin2bcd(uint16_t value, uint8_t buffer[4]) {
    buffer[0] = 0;
    while (value > 999) {
        value -= 1000;
        buffer[0]++;
    }
    
    buffer[1] = 0;
    while (value > 99) {
        value -= 100;
        buffer[1]++;
    }
    
    buffer[2] = 0;
    while (value > 9) {
        value -= 10;
        buffer[2]++;
    }
    
    buffer[3] = value;
}

void bcd2text(uint8_t ints[4]) {
    uint8_t i;
    for(i = 0; i < 4; i++) {
        ints[i] += '0';
    }
}
