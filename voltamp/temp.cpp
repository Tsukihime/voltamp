#include "temp.h"
#include "DS18B20_defs.h"

extern "C" {
#include "lib/1-wire-avr-lib/OWIPolled.h"
#include "lib/1-wire-avr-lib/OWIHighLevelFunctions.h"
#include "lib/1-wire-avr-lib/OWIBitFunctions.h"
#include "lib/1-wire-avr-lib/common_files/OWIcrc.h"
};

#define BUS OWI_PIN_5

TemperatureSensor temperatureSensor;

void TemperatureSensor::initialize() {
    OWI_Init(BUS);
    OWI_SendByte(DS18B20_WRITE_SCRATCHPAD, BUS);

    OWI_SendByte(0, BUS);
    OWI_SendByte(0, BUS);
    OWI_SendByte(DS18B20_RES_12BIT, BUS);
}

void TemperatureSensor::startMeasurement() {
    OWI_Init(BUS);
    OWI_DetectPresence(BUS);
    OWI_SkipRom(BUS);
    OWI_SendByte(DS18B20_CONVERT_T, BUS);
}

int16_t TemperatureSensor::readRawTemperature() {    
    /*wait while semsor complete measurement*/
    //while (!OWI_ReadBit(BUS));

    OWI_DetectPresence(BUS);
    OWI_SkipRom(BUS);
    OWI_SendByte(DS18B20_READ_SCRATCHPAD, BUS);

    uint8_t LS = OWI_ReceiveByte(BUS);
    uint8_t MS = OWI_ReceiveByte(BUS);

    int16_t data = ((MS << 8) | LS);
    return data;
}
