//
// Created by Administrator on 25-4-2.
//

#include "Timer.h"

void onInterrupt()
{
    Timer::getInstance()._timerFlag = true;
}

Timer::Timer()
:_interruptTimer(nullptr)
,_timerFlag(false)
{

}

Timer& Timer::getInstance()
{
    static Timer instance;
    return instance;
}

void Timer::begin()
{
    _interruptTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(_interruptTimer, &onInterrupt, true);  // 将中断函数 onTimer 绑定到定时器
    timerAlarmWrite(_interruptTimer, 1000*1000, true);  // 设置定时器中断周期为 0.5秒 , @2以微秒为单位
    timerAlarmEnable(_interruptTimer);  // 启动定时器
}



