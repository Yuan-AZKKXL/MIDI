#include <Arduino.h>
#include "Event/ButtonState.h"
#include "Button/EmmaButton.hpp"

//创建音序器
SAM2695Synth seq = SAM2695Synth::getInstance();
//创建状态机
StateMachine stateMachine;
//创建按钮
EmmaButton button;
int beatCount = 0;                // 打拍计数器
unsigned long previousMillis = 0; // 记录上一次发送MIDI信号的时间
int noteType = QUATER_NOTE;                 // 音符类型选择，0（四分音符）、1（八分音符）、2（十六音符）
int beatsPerBar = BEATS_BAR_DEFAULT;              // 每小节拍数，可以是2、3或4
bool isPressed = false;

void multiTrackPlay()
{
    unsigned long interval = (BASIC_TIME / seq.getBpm()) / (noteType + 1);
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        if(channel_1_on_off_flag)
        {
            seq.play(CHANNEL_0);
        }
        if(channel_2_on_off_flag)
        {
            seq.play(CHANNEL_1);
        }
        if(channel_3_on_off_flag)
        {
            seq.play(CHANNEL_2);
        }
        if(channel_4_on_off_flag)
        {
            seq.play(CHANNEL_3);
        }
        // if(drum_on_off_flag)
        // {
        //     seq.drumPlay();
        // }
    }
}

// 获取下一个输入事件
Event* getNextEvent()
{
    //用来控制长按和短按事件，松开的时候才会触发短按，否则是长按
    if(button.A.pressed()==BtnAct::Pressed
        || button.B.pressed()==BtnAct::Pressed
        || button.C.pressed()==BtnAct::Pressed
        || button.D.pressed()==BtnAct::Pressed)
    {
        isPressed = true;
    }

    if(button.A.longPressed()==BtnAct::LongPressed)
    {
        isPressed = false;
        Serial.println("Button A long pressed");
        Event* e = new Event(EventType::BtnALongPressed);
        return e;
    }

    if(button.B.longPressed()==BtnAct::LongPressed)
    {
        isPressed = false;
        Serial.println("Button B long pressed");
        Event* e = new Event(EventType::BtnBLongPressed);
        return e;
    }

    if(button.C.longPressed()==BtnAct::LongPressed)
    {
        isPressed = false;
        Serial.println("Button C long pressed");
        Event* e = new Event(EventType::BtnCLongPressed);
        return e;
    }

    if(button.D.longPressed()==BtnAct::LongPressed)
    {
        isPressed = false;
        Serial.println("Button D long pressed");
        Event* e = new Event(EventType::BtnDLongPressed);
        return e;
    }

    if(button.A.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Serial.println("Button A pressed");
            Event* e = new Event(EventType::BtnAPressed);
            return e;
        }
    }
    if(button.B.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Serial.println("Button B pressed");
            Event* e = new Event(EventType::BtnBPressed);
            return e;
        }
    }
    if(button.C.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Serial.println("Button C pressed");
            Event* e = new Event(EventType::BtnCPressed);
            return e;
        }
    }
    if(button.D.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Serial.println("Button D pressed");
            Event* e = new Event(EventType::BtnDPressed);
            return e;
        }
    }

    return nullptr;
}

void setup() {
    Serial.begin(115200);
    delay(3000);
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



