#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

Timer timer;

#define TCNT_INIT_VALUE (256 - (F_CPU / 1000 / 64)) // 0.001 sec

ISR(TIMER0_OVF_vect) {
    // ISR Prologue and write TCNT0 takes 34 cycles,
    // interrupt call time 4-15 cycles
    // timer prescaler is 64 and that more than 34 + 15
    // Therefore, we are able to set TCNT0 before the timer is triggered at least once
    TCNT0 = TCNT_INIT_VALUE;
    timer.updateTaskStatus();
}

void Timer::updateTaskStatus() {
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
    TCNT0 = TCNT_INIT_VALUE;
    TCCR0 = (0 << CS02) | (1 << CS01) | (1 << CS00); // clock source = clkIO/64 From prescaler
    TIMSK |= (1 << TOIE0); // Timer/Counter0 Overflow Interrupt Enable.
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
