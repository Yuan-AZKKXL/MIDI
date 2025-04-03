#include <Arduino.h>
#include "Event/ButtonState.hpp"
#include "Button/EmmaButton.hpp"
#include "SAM2695/SAM2695Synth.h"

#define STATE_1_LED_TIME 800
#define STATE_2_LED_TIME 400
#define STATE_3_LED_TIME 200

//创建音序器
SAM2695Synth synth = SAM2695Synth::getInstance();
//创建状态机
StateMachine stateMachine;
//创建按钮
EmmaButton button;
StateManager* manager = StateManager::getInstance();


int beatCount = 0;                // 打拍计数器
unsigned long previousMillis = 0; // 记录上一次发送MIDI信号的时间
int noteType = QUATER_NOTE;                 // 音符类型选择，0（四分音符）、1（八分音符）、2（十六音符）
int beatsPerBar = BEATS_BAR_DEFAULT;              // 每小节拍数，可以是2、3或4
bool isPressed = false;
uint8_t drupCount = 0;
uint8_t voice = 50;
uint8_t  modeID = 0;//模式ID
int ledTime = 0;//LED反转时间，500ms
unsigned long previousMillis2 = 0;//记录上一次灯的时间

// 获取下一个输入事件
Event* getNextEvent()
{
    //Used to control long press and short press events.
    //A short press is triggered when released, otherwise
    //it is considered a long press.
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
        Event* e = new Event(EventType::BtnALongPressed);
        return e;
    }

    if(button.B.longPressed()==BtnAct::LongPressed)
    {
        isPressed = false;
        Event* e = new Event(EventType::BtnBLongPressed);
        return e;
    }

    if(button.C.longPressed()==BtnAct::LongPressed)
    {
        isPressed = false;
        Event* e = new Event(EventType::BtnCLongPressed);
        return e;
    }

    if(button.D.longPressed()==BtnAct::LongPressed)
    {
        isPressed = false;
        Event* e = new Event(EventType::BtnDLongPressed);
        return e;
    }

    if(button.A.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Event* e = new Event(EventType::BtnAPressed);
            return e;
        }
    }
    if(button.B.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Event* e = new Event(EventType::BtnBPressed);
            return e;
        }
    }
    if(button.C.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Event* e = new Event(EventType::BtnCPressed);
            return e;
        }
    }
    if(button.D.released()==BtnAct::Released)
    {
        if(isPressed)
        {
            isPressed = false;
            Event* e = new Event(EventType::BtnDPressed);
            return e;
        }
    }

    return nullptr;
}
// 多轨播放
void multiTrackPlay()
{
    unsigned long interval = (BASIC_TIME / synth.getBpm()) / (noteType + 1);
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        if(channel_1_on_off_flag)
        {
            uint8_t pitch = synth.getPitch();
            synth.setNoteOn(CHANNEL_0,synth.getPitch(),voice);
        }
        if(channel_2_on_off_flag)
        {
            synth.setNoteOn(CHANNEL_1,synth.getPitch(),voice);
        }
        if(channel_3_on_off_flag)
        {
            synth.setNoteOn(CHANNEL_2,synth.getPitch(),voice);
        }
        if(channel_4_on_off_flag)
        {
            synth.setNoteOn(CHANNEL_3,synth.getPitch(),voice);
        }
        if(drum_on_off_flag)
        {
            if(drupCount % 4 == 0)
            {
                synth.setNoteOn(CHANNEL_9,NOTE_C2,voice);
                synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
            }
            else if(drupCount % 4 == 1)
            {
                synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
            }
            else if(drupCount % 4 == 2)
            {
                synth.setNoteOn(CHANNEL_9,NOTE_D2,voice);
                synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
            }
            else if(drupCount % 4 == 3)
            {
                synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
            }
            drupCount++;
        }
    }
}
//灯光显示
void ledShow()
{
    modeID = stateMachine.getCurrentState()->getID();
    if(modeID == ButtonState1::ID)
    {
        ledTime = STATE_1_LED_TIME;
    }
    else if(modeID == ButtonState2::ID)
    {
        ledTime = STATE_2_LED_TIME;
    }
    else if(modeID == ButtonState3::ID)
    {
        ledTime = STATE_3_LED_TIME;
    }
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis2 >= ledTime)
    {
        previousMillis2 = millis();
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
}

void setup() {
    //初始化串口
    Serial.begin(115200);
    //初始化LED
    pinMode(LED_BUILTIN, OUTPUT);
    delay(3000);
    //初始化音序器
    synth.begin();
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
    ledShow();
}


