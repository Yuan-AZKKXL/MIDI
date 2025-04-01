#include <Arduino.h>
#include "Button/EmmaButton.hpp"
#include "Event/ButtonState.hpp"

EmmaButton button;

//创建状态机
StateMachine stateMachine;
// 获取下一个输入事件
Event* getNextEvent() {
    if (button.A.pressed() == BtnAct::Pressed)
    {
        Event* event = new Event(EventType::BtnAPressed);
        return event;
    }
    if (button.A.released() == BtnAct::Released)
    {
        Event* event = new Event(EventType::BtnAReleased);
        return event;
    }

    if (button.B.pressed() == BtnAct::Pressed)
    {
        Event* event = new Event(EventType::BtnBPressed);
        return event;
    }
    if (button.B.released() == BtnAct::Released)
    {
        Event* event = new Event(EventType::BtnBReleased);
        return event;
    }

    if (button.C.pressed() == BtnAct::Pressed)
    {
        Event* event = new Event(EventType::BtnCPressed);
        return event;
    }
    if (button.C.released() == BtnAct::Released)
    {
        Event* event = new Event(EventType::BtnCReleased);
        return event;
    }

    if (button.D.pressed() == BtnAct::Pressed)
    {
        Event* event = new Event(EventType::BtnDPressed);
        return event;
    }
    if (button.D.released() == BtnAct::Released)
    {
        Event* event = new Event(EventType::BtnDReleased);
        return event;
    }
    return nullptr;
}

void setup()
{
    //注册状态
    StateManager* manager = StateManager::getInstance();
    //注册按钮状态
    manager->registerState(new ButtonState());
    //注册错误状态
    ErrorState* errorState = new ErrorState();
    manager->registerState(errorState);
    //初始化状态机
    if(!stateMachine.init(manager->getState(ButtonState::ID), errorState));
    {
        StateManager::releaseInstance();
        return ;
    }
}

void loop()
{
    // 获取下一个事件
    Event* event = getNextEvent();
    if(event != nullptr)
    {
        stateMachine.handleEvent(event);
        delete event;
    }
}

