#ifndef TIMER_H_
#define TIMER_H_

#define MAX_TIMER_TASK_COUNT 4

typedef void(*TProcedurePointer)();

extern "C" void TIMER0_OVF_vect(void) __attribute__((signal));

class Timer {
    friend void TIMER0_OVF_vect();

    public:
        void initialize();
        void processTasks();       
        bool addTask(uint16_t period_ms, TProcedurePointer callback);

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
