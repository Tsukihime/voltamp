#include "temp.h"
#include "DS18B20_defs.h"
#include "config.h"

extern "C" {
#include "lib/1-wire-avr-lib/OWIPolled.h"
#include "lib/1-wire-avr-lib/OWIHighLevelFunctions.h"
#include "lib/1-wire-avr-lib/OWIBitFunctions.h"
#include "lib/1-wire-avr-lib/common_files/OWIcrc.h"
}

TemperatureSensor temperatureSensor;

void TemperatureSensor::initialize() {
    OWI_Init(TEMP_PIN);
    OWI_SendByte(DS18B20_WRITE_SCRATCHPAD, TEMP_PIN);

    OWI_SendByte(0, TEMP_PIN);
    OWI_SendByte(0, TEMP_PIN);
    OWI_SendByte(DS18B20_RES_12BIT, TEMP_PIN);
}

void TemperatureSensor::startMeasurement() {
    OWI_Init(TEMP_PIN);
    OWI_DetectPresence(TEMP_PIN);
    OWI_SkipRom(TEMP_PIN);
    OWI_SendByte(DS18B20_CONVERT_T, TEMP_PIN);
}

int16_t TemperatureSensor::readRawTemperature() {    
    /*wait while semsor complete measurement*/
    //while (!OWI_ReadBit(BUS));

    OWI_DetectPresence(TEMP_PIN);
    OWI_SkipRom(TEMP_PIN);
    OWI_SendByte(DS18B20_READ_SCRATCHPAD, TEMP_PIN);

    uint8_t LS = OWI_ReceiveByte(TEMP_PIN);
    uint8_t MS = OWI_ReceiveByte(TEMP_PIN);

    int16_t data = ((MS << 8) | LS);
    return data;
}
