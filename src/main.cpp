#include <Arduino.h>
#include "Event/ButtonState.h"
#include "Button/EmmaButton.hpp"
#include "Button/Button/ButtonInterrupt.h"

SAM2695Synth seq = SAM2695Synth::getInstance();
//创建状态机
StateMachine stateMachine;
//创建按钮
EmmaButton button;
//定时器
hw_timer_t *timer = nullptr;
bool timeFlag = false;

void IRAM_ATTR onTimer() {
    // 每秒触发一次，执行的操作
    timeFlag = true;
}

void multiTrackPlay()
{
    if(timeFlag)
    {
        timeFlag = false;
        if(channel_1_flag)
        {
            seq.play(CHANNEL_0);
        }
        if(channel_2_flag)
        {
            seq.play(CHANNEL_1);
        }
        if(channel_3_flag)
        {
            seq.play(CHANNEL_2);
        }
        if(channel_4_flag)
        {
            seq.play(CHANNEL_3);
        }
    }
}

// 获取下一个输入事件
Event* getNextEvent() {
    if(button.A.pressed()==BtnAct::Pressed)
    {
        Serial.println("Button A pressed");
        Event* e = new Event(EventType::BtnAPressed);
        return e;
    }

    if(button.B.pressed()==BtnAct::Pressed)
    {
        Serial.println("Button B pressed");
        Event* e = new Event(EventType::BtnBPressed);
        return e;
    }

    if(button.C.pressed()==BtnAct::Pressed)
    {
        Serial.println("Button C pressed");
        Event* e = new Event(EventType::BtnCPressed);
        return e;
    }

    if(button.D.pressed()==BtnAct::Pressed)
    {
        Serial.println("Button D pressed");
        Event* e = new Event(EventType::BtnDPressed);
        return e;
    }

    if(button.A.longPressed()==BtnAct::LongPressed)
    {
        Serial.println("Button A long pressed");
        Event* e = new Event(EventType::BtnALongPressed);
        return e;
    }

    if(button.B.longPressed()==BtnAct::LongPressed)
    {
        Serial.println("Button B long pressed");
        Event* e = new Event(EventType::BtnBLongPressed);
        return e;
    }

    if(button.C.longPressed()==BtnAct::LongPressed)
    {
        Serial.println("Button C long pressed");
        Event* e = new Event(EventType::BtnCLongPressed);
        return e;
    }

    if(button.D.longPressed()==BtnAct::LongPressed)
    {
        Serial.println("Button D long pressed");
        Event* e = new Event(EventType::BtnDLongPressed);
        return e;
    }

    return nullptr;
}

void setup() {
    Serial.begin(115200);
    delay(3000);
    // pinMode(EMMA_BTN_A_PIN, INPUT_PULLUP);
    // pinMode(EMMA_BTN_B_PIN, INPUT_PULLUP);
    // pinMode(EMMA_BTN_C_PIN, INPUT_PULLUP);
    // pinMode(EMMA_BTN_D_PIN, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(EMMA_BTN_A_PIN), buttonAInterrupt, FALLING);
    // attachInterrupt(digitalPinToInterrupt(EMMA_BTN_B_PIN), buttonBInterrupt, FALLING);
    // attachInterrupt(digitalPinToInterrupt(EMMA_BTN_C_PIN), buttonCInterrupt, FALLING);
    // attachInterrupt(digitalPinToInterrupt(EMMA_BTN_D_PIN), buttonDInterrupt, FALLING);
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);  // 将中断函数 onTimer 绑定到定时器
    timerAlarmWrite(timer, 1000*1000, true);  // 设置定时器中断周期为 0.5秒 , @2以微秒为单位
    timerAlarmEnable(timer);  // 启动定时器
    //初始化音序器
    seq.begin();
    //注册状态
    StateManager* manager = StateManager::getInstance();
    //注册按钮状态
    manager->registerState(new ButtonState1());
    manager->registerState(new ButtonState2());
    manager->registerState(new ButtonState3());
    //注册错误状态
    ErrorState* errorState = new ErrorState();
    manager->registerState(errorState);
    //初始化状态机
    if(!(stateMachine.init(manager->getState(ButtonState1::ID), errorState)))
    {
        StateManager::releaseInstance();
        return ;
    }

    Serial.println("Button and Timer ready!");
}

void loop() {
    Event* event = getNextEvent();
    //如果有输入事件，则处理
    if(event)
    {
        stateMachine.handleEvent(event);
        delete event;
    }
    multiTrackPlay();
}



