#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "adc.h"

Adc adc;

void Adc::initialize() {
    ADMUX = (0 << REFS1) | (0 << REFS0) | // external reference on AREF, Internal Vref turned off
            (0 << ADLAR);                 // result is right adjusted

    
    ADCSRA = (1 << ADEN) |                               // ADC Enable
             (1 << ADIE) |                               // ADC Interrupt Enable
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC Prescaler of 128

    MCUCR = (1 << SE) |                           // Sleep Enable
            (0 << SM2) | (0 << SM1) | (1 << SM0); // ADC Noise Reduction
}

// ADC Interrupt Is Used To Wake Up CPU From Sleep Mode
EMPTY_INTERRUPT(ADC_vect)

#define ADCMUX_MASK 0x07

// return 10 bit measure
uint16_t Adc::getValue(uint8_t channel) {
    channel &= ADCMUX_MASK;
    ADMUX = (ADMUX & ~ADCMUX_MASK) | channel;
    
    // Enter Sleep Mode To Trigger ADC Measurement
    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sleep_cpu();
    // CPU Will Wake Up From ADC Interrupt
    sleep_disable();
    return ADC;
}

#define ADC_OVERSAMPLE_EXTRA_BIT_COUNT 2
#define ADC_OVERSAMPLE_MEASURE_COUNT 16 //4 ^ ADC_OVERSAMPLE_EXTRA_BIT_COUNT

uint16_t Adc::getOversampled12bitValue(uint8_t channel) {
    uint16_t summa = 0;
    for (uint8_t i = 0; i < ADC_OVERSAMPLE_MEASURE_COUNT; i++) {
        summa += getValue(channel);
    }
    return (summa >> ADC_OVERSAMPLE_EXTRA_BIT_COUNT);
}
