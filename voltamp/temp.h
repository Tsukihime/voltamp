#ifndef TEMP_H_
#define TEMP_H_

#include <stdint.h>

class TemperatureSensor {
    public:
        void initialize();
        void startMeasurement();
        int16_t readRawTemperature();
};

extern TemperatureSensor temperatureSensor;

#endif /* TEMP_H_ */
