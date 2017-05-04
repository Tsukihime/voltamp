#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "timer.h"

Timer timer;

ISR(TIMER2_COMP_vect) {
    timer.updateTaskStatus();
}

#define TIMER2_PRESCALER_DIV64 ((1 << CS22) | (0 << CS21) | (0 << CS20))
#define TIMER2_CTC_MODE ((1 << WGM21) | (0 << WGM20))
#define COMPARE_RESET_DIV64_1MS_VALUE ((F_CPU / 1000 / 64) - 1)

void Timer::initialize() {
    taskCount = 0;
    TCCR2 = TIMER2_PRESCALER_DIV64 | TIMER2_CTC_MODE;
    OCR2 = COMPARE_RESET_DIV64_1MS_VALUE;
    TCNT2 = 0;
    enableInterrupts();
}

void Timer::processTasks() {
    for(uint8_t i = 0; i < taskCount; i++){
        if(tasks[i].isReady) {
            tasks[i].isReady = false;
            tasks[i].callback();
        }
    }
}
#include "utils.h"
void Timer::run() {
    while(true) {
        processTasks();        
        set_sleep_mode(SLEEP_MODE_IDLE); // Enter Sleep Mode To Save Power
        sleep_mode();                    // CPU Will Wake Up From Timer2 Interrupt
    }
}

bool Timer::addTask(uint16_t period_ms, TProcedurePointer callback)
{
    if (taskCount >= MAX_TIMER_TASK_COUNT) {
        return false;
    }

    tasks[taskCount].callback = callback;
    tasks[taskCount].period = period_ms;
    tasks[taskCount].counter = 0;
    tasks[taskCount].isReady = false;

    taskCount++;

    return true;
}
