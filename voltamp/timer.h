#ifndef TIMER_H_
#define TIMER_H_

typedef void(*TPROC)();

typedef struct {
        uint16_t period;
        uint16_t counter;
        TPROC callback;    
} TimerTask;

class Timer {
    public:
        void initialize();
        void sync(TimerTask tasks[], uint8_t taskCount);
        TimerTask makeTask(uint16_t period_ms, TPROC callback);
    
    private:
        void processTask(TimerTask *task);
        void waitNextTick();
};

extern Timer timer;

#endif /* TIMER_H_ */
