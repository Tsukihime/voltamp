#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

Timer timer;

ISR(TIMER2_COMP_vect) {
    timer.ISRTimerFired();
}

void Timer::ISRTimerFired() {
    for (uint8_t i = 0; i < taskCount; i++) {
        tasks[i].counter++;
        if(tasks[i].counter >= tasks[i].period) {
            tasks[i].counter = 0;
            tasks[i].isReady = true;
        }
    }
}

void Timer::initialize() {
    taskCount = 0;

    TCCR2 = (1 << CS22) | (0 << CS21) | (0 << CS20); // prescaler 64
    TCCR2 |= (1 << WGM21) | (0 << WGM20);            // CTC mode
    OCR2 = (F_CPU / 1000 / 64);                      // compare reset value 0,001 sec
    TCNT2 = 0;
    TIMSK |= (1 << OCIE2); // Timer/Counter2 Output Compare Match Interrupt Enable
}

void Timer::processTasks() {
    for(uint8_t i = 0; i < taskCount; i++){
        if(tasks[i].isReady) {
            tasks[i].isReady = false;
            tasks[i].callback();
        }
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
