#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

Timer timer;
volatile bool mainTimerFired;

ISR(TIMER2_COMP_vect) {
    mainTimerFired = true;
}

void Timer::initialize() {
    TCCR2 = (1 << CS22) | (0 << CS21) | (0 << CS20); // prescaler 64
    TCCR2 |= (1 << WGM21) | (0 << WGM20);            // CTC mode
    OCR2 = (F_CPU / 1000 / 64);                      // compare reset value 0,001 sec
    TCNT2 = 0;
    TIMSK |= (1 << OCIE2); // Timer/Counter2 Output Compare Match Interrupt Enable
}

void Timer::waitNextTick() {    
    while(!mainTimerFired) {
        // wait until timer fired
    }
    mainTimerFired = false; // reset fired flag
}

void Timer::processTask(TimerTask *task) {
    task->counter++; 
    if(task->counter >= task->period) {
        task->counter = 0;
        task->callback();
    }
}

TimerTask Timer::makeTask(uint16_t period_ms, TPROC callback) {
    TimerTask res;
    res.callback = callback;
    res.period = period_ms;
    res.counter = 0;
    return res;
}

void Timer::sync(TimerTask tasks[], uint8_t task_count) {
    uint8_t i;
    waitNextTick();
    for(i = 0; i < task_count; i++){
        processTask(&tasks[i]);
    }
}
