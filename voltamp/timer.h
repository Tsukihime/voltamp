#ifndef TIMER_H_
#define TIMER_H_

#define MAX_TIMER_TASK_COUNT 4

typedef void(*TProcedurePointer)();

extern "C" void TIMER2_COMP_vect(void) __attribute__((signal));

class Timer {
    friend void TIMER2_COMP_vect();

    public:
        void initialize();
        void processTasks();
        void run();

        bool addTask(uint16_t period_ms, TProcedurePointer callback);

    private:
        typedef struct {
            uint16_t period;
            uint16_t counter;
            bool isReady;
            TProcedurePointer callback;
        } TimerTask;

        TimerTask tasks[MAX_TIMER_TASK_COUNT];
        uint8_t taskCount;

        inline void updateTaskStatus() {
            for (uint8_t i = 0; i < taskCount; i++) {
                tasks[i].counter++;
                if(tasks[i].counter >= tasks[i].period) {
                    tasks[i].counter = 0;
                    tasks[i].isReady = true;
                }
            }
        }
};

extern Timer timer;

static inline void __timsk_clean_up(uint8_t *timskOldValue) {
    TIMSK = *timskOldValue;
}

static inline uint8_t __disaleTimerInterrupts() {
    TIMSK &= ~((1 << OCIE2) | (1 << TOIE2));
    return 1;
}

#define TIMER_NO_ISR_BLOCK for ( uint8_t timsk_save __attribute__((__cleanup__(__timsk_clean_up))) = TIMSK, \
__ToDo = __disaleTimerInterrupts(); __ToDo ; __ToDo = 0 )

#endif /* TIMER_H_ */
