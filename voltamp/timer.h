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

        inline void disableInterrupts() {
            TIMSK &= ~(1 << OCIE2); // Timer/Counter2 Output Compare Match Interrupt Disable
        }

        inline void enableInterrupts() {
            TIMSK |= (1 << OCIE2); // Timer/Counter2 Output Compare Match Interrupt Enable
        }

    private:
        typedef struct {
            uint16_t period;
            uint16_t counter;
            bool isReady;
            TProcedurePointer callback;
        } TimerTask;

        volatile TimerTask tasks[MAX_TIMER_TASK_COUNT];
        volatile uint8_t taskCount;

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

#endif /* TIMER_H_ */
