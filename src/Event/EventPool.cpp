//
// Created by Administrator on 25-4-9.
//

#include "EventPool.h"

EventPool::EventPool()
    :currentEventIndex(0)
{
    for (int i = 0; i < EVENT_POOL_SIZE; i++) {
        eventPool[i] = Event();  // 默认事件类型为 None
    }
}

EventPool::~EventPool()
{

}

Event* EventPool::getEvent(EventType type)
{
    // 尝试从池中获取一个未使用的事件
    for (int i = 0; i < EVENT_POOL_SIZE; i++) {
        if (!eventPool[i].isInUse()) {
            eventPool[i].setType(type);
            eventPool[i].setInUse(true);  // 标记事件为正在使用
            return &eventPool[i];
        }
    }
    // 如果池满了，返回空指针
    return nullptr;
}

void EventPool::recycleEvent(Event* event)
{
    if (event) {
        event->setInUse(false);
    }
}

void EventPool::reset()
{
    // 重置事件池，回收所有事件
    for (int i = 0; i < EVENT_POOL_SIZE; i++) {
        eventPool[i].setInUse(false);
    }
    currentEventIndex = 0;
}


