#ifndef POWERCOUNTER_H_
#define POWERCOUNTER_H_

#define TIME_SEGMENT_COUNT 36000; // 1 hour / 0.1sec

class PowerCounter {
    private:
    uint32_t milliAmperesAll;
    uint32_t milliWattsAll;

    public:
    inline void reset() {
        milliAmperesAll = 0;
        milliWattsAll = 0;
    }

    inline void appendMeasureEvery100ms(uint16_t milliamperes, uint16_t milliwatts) {
        milliAmperesAll += milliamperes;
        milliWattsAll += milliwatts;
    }

    inline uint32_t getMilliAmperePerHour() {
        return milliAmperesAll / TIME_SEGMENT_COUNT;
    }

    inline uint32_t getMilliWattsPerHour() {
        return milliWattsAll / TIME_SEGMENT_COUNT;
    }

};

#endif /* POWERCOUNTER_H_ */