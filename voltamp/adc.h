#ifndef _ADC_H_
#define _ADC_H_

class Adc {
    public:
        void initialize();
        uint16_t getValue(uint8_t channel);
        uint16_t getOversampled12bitValue(uint8_t channel);
};

extern Adc adc;

#endif /* _ADC_H_ */
