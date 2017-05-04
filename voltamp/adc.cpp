#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "timer.h"
#include "utils.h"

Adc adc;

#define ADCMUX_MASK 0x07
static void selectAdcChannel(uint8_t channel) {
    channel &= ADCMUX_MASK;
    ADMUX = (ADMUX & ~ADCMUX_MASK) | channel;
}

// ADC Interrupt Is Used To Wake Up CPU From Sleep Mode
EMPTY_INTERRUPT(ADC_vect)

static uint16_t getAdcValue() {
    timer.disableTimerInterrupts();      // To prevent timer wakeup CPU before ADC complete measurement
    set_sleep_mode(SLEEP_MODE_ADC); // Enter Sleep Mode To Trigger ADC Measurement
    sleep_mode();                   // CPU Will Wake Up From ADC Interrupt
    timer.enableTimerInterrupts();
    return ADC;
}

#define ADC_EXTERNAL_AREF ((0 << REFS1) | (0 << REFS0))
#define ADC_RESULT_RIGHT_ADJUSTED (0 << ADLAR)
#define ADC_ENABLE (1 << ADEN)
#define ADC_INTERRUPT_ENABLE (1 << ADIE)
#define ADC_PRESCALER_DIV128 ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))

void Adc::initialize() {
    ADMUX = ADC_EXTERNAL_AREF | ADC_RESULT_RIGHT_ADJUSTED;
    ADCSRA = ADC_INTERRUPT_ENABLE | ADC_PRESCALER_DIV128;
}

inline void adcEnable() {
    SET_PORT_BIT(ADCSRA, ADEN);
}

inline void adcDisable() {
    CLR_PORT_BIT(ADCSRA, ADEN);
}

/**
    @param channel adc channel {0..7}
    @return 10 bit measure
*/
uint16_t Adc::getValue(uint8_t channel) {
    adcEnable();
    selectAdcChannel(channel);
    return getAdcValue();
    adcDisable();
}

/**
    see Application Note AVR121 Enhancing ADC resolution by oversampling
    @param channel adc channel {0..7}
    @param extraBitCount {1..6} additional bits
    @return (10 + extraBitCount)bit measured value.
*/
uint16_t Adc::getOversampledValue(uint8_t channel, uint8_t extraBitCount) {
    adcEnable();
    selectAdcChannel(channel);
    uint32_t summa = 0;
    uint16_t measureCount = 1 << (extraBitCount * 2); // 4^n = 2^(2n) = (1 << 2n)
    while(measureCount--) {
        summa += getAdcValue();
    }
    adcDisable();
    return (summa >> extraBitCount);
}
