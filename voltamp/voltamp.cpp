#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>

#include "display.h"
#include "displayFormat.h"
#include "adc.h"
#include "uart.h"
#include "timer.h"
#include "temp.h"
#include "utils.h"
#include "config.h"

int16_t q4Temperature = 0; ///< fixed point value with 4 bit exponent

void updateMainDisplay(uint16_t millivolts, uint16_t milliamperes) {
    TDisplay voltsDisplay;    
    Format::volts(millivolts, &voltsDisplay);
    
    TDisplay amperesDisplay;
    Format::amperes(milliamperes, &amperesDisplay);

    display.sendToDisplay(&voltsDisplay, &amperesDisplay);
}

void updateAltDisplay(int16_t degrees, uint16_t milliwatts) {
    TDisplay degreesDisplay;    
    Format::degrees(degrees, &degreesDisplay);
    
    TDisplay wattsDisplay;
    Format::watts(milliwatts, &wattsDisplay);

    display.sendToDisplay(&degreesDisplay, &wattsDisplay);
}

static inline bool isAltButtonPressed() {
    return GET_PORT_BIT(PIND, BUTTON_ALT_PIN);
}

void updateRectifierVoltage(uint16_t millivolts) {
    if (millivolts > RECTIFIER_UP_VOLTAGE) {
        SET_PORT_BIT(PORTD, RECTIFIER_PIN);
    }
    if (millivolts < RECTIFIER_DOWN_VOLTAGE) {
        CLR_PORT_BIT(PORTD, RECTIFIER_PIN);
    }
}

void updateFanState(int8_t temperature) {
    if (temperature >= FAN_ON_TEMP) {
        SET_PORT_BIT(PORTD, FAN_PIN);
    }
    if (temperature <= FAN_OFF_TEMP) {
        CLR_PORT_BIT(PORTD, FAN_PIN);
    }
}

void processVoltageMeasurement() {
    uint16_t millivolts = adc.getOversampledValue(VOLTS_ADC_CHANNEL, 4); // 16368 = 16.368 V
    uint16_t ampAdcValue = adc.getOversampledValue(AMPS_ADC_CHANNEL, 3); // 8184 = 2.046 A

    uint16_t milliamperes = (ampAdcValue + 2) / 4;

    if(millivolts != 0) {
        millivolts += 20; // volts offset
    }


    updateRectifierVoltage(millivolts);
    
    int16_t degrees = (q4Temperature * 10) >> 4;

    if(isAltButtonPressed()){
        uint16_t milliwatts = (uint32_t)millivolts * (uint32_t)milliamperes / 1000;
        updateAltDisplay(degrees, milliwatts);
    } else {
        updateMainDisplay(millivolts, milliamperes);
    }

    uart.updateData(millivolts, milliamperes, degrees);
}

void processRadiatorTemperature() {
    static bool isFirstCall = true;

    if(isFirstCall) {
        temperatureSensor.startMeasurement();
        isFirstCall = false;
        return;
    }

    q4Temperature = temperatureSensor.readTemperature();
    temperatureSensor.startMeasurement();
    int8_t radiatorTemperature = q4Temperature >> 4;
    updateFanState(radiatorTemperature);
}

#define ALL_INPUT (0x00)
#define ALL_PULLUP (0xff)
static void voltampInitialize() {
    DDRB = ALL_INPUT;
    DDRC = ALL_INPUT;
    DDRD = (1 << RECTIFIER_PIN) | (1 << FAN_PIN);

    PORTB = ALL_PULLUP;
    PORTC = ALL_PULLUP & ~((1 << VOLTS_ADC_CHANNEL) | (1 << AMPS_ADC_CHANNEL));
    PORTD = ALL_PULLUP & ~((1 << RECTIFIER_PIN) | (1 << FAN_PIN));
}

void initAll() {
    voltampInitialize();
    adc.initialize();
    uart.initialize();
    display.initialize();
    timer.initialize();
    temperatureSensor.initialize();

    sei();
}

int main(void) {
    initAll();

    timer.addTask(100, &processVoltageMeasurement);
    timer.addTask(800, &processRadiatorTemperature);
    timer.run();
}
