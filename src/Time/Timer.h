//
// Created by Administrator on 25-4-2.
//

#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

void onInterrupt();

class Timer {
public:
    static Timer& getInstance();
    void begin();

private:
    Timer();
public:
    hw_timer_t *_interruptTimer;
    bool _timerFlag;
};



#endif //TIMER_H
