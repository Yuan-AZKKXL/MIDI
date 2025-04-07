#include <Arduino.h>
#include "Event/ButtonState.hpp"
#include "SAM2695/SAM2695Synth.h"
#include "Button/Button.h"

//定义不同模式下的指示灯
#define STATE_1_LED_TIME 800
#define STATE_2_LED_TIME 400
#define STATE_3_LED_TIME 200

//定义按钮所需要的结构体
BtnState btnA = {HIGH, HIGH, 0, 0, false};
BtnState btnB = {HIGH, HIGH, 0, 0, false};
BtnState btnC = {HIGH, HIGH, 0, 0, false};
BtnState btnD = {HIGH, HIGH, 0, 0, false};

//创建音序器
SAM2695Synth synth = SAM2695Synth::getInstance();

//创建状态机
StateMachine stateMachine;
StateManager* manager = StateManager::getInstance();

//节拍相关
int beatCount = 0;                                  // 打拍计数器
unsigned long preMillisCh_1 = 0;                    // 记录轨道1上一次发送MIDI信号的时间
unsigned long preMillisCh_2 = 0;                    // 记录轨道2上一次发送MIDI信号的时间
unsigned long preMillisCh_3 = 0;                    // 记录轨道2上一次发送MIDI信号的时间
unsigned long preMillisCh_4 = 0;                    // 记录轨道2上一次发送MIDI信号的时间
int noteType = QUATER_NOTE;                         // 音符类型选择，0（四分音符）、1（八分音符）、2（十六音符）
int beatsPerBar = BEATS_BAR_DEFAULT;                // 每小节拍数，可以是2、3或4
uint8_t drupCount = 0;                              //鼓点轨道播放计数器
uint8_t voice = (VELOCITY_MAX + VELOCITY_MIN) / 2;

//指示灯状态
uint8_t  modeID = State1::ID;                       //模式ID
int ledTime = STATE_1_LED_TIME;                     //LED反转时间，500ms
unsigned long previousMillisLED = 0;                //记录上一次灯的时间

//获取下一个事件
Event* getNextEvent()
{
    // detectButtonEvents(BUTTON_A_PIN, btnA, act);
    detectButtonEvents(BUTTON_A_PIN, btnA, shortPressFlag_A, longPressFlag_A, releaseFlag_A);
    detectButtonEvents(BUTTON_B_PIN, btnB, shortPressFlag_B, longPressFlag_B, releaseFlag_B);
    detectButtonEvents(BUTTON_C_PIN, btnC, shortPressFlag_C, longPressFlag_C, releaseFlag_C);
    detectButtonEvents(BUTTON_D_PIN, btnD, shortPressFlag_D, longPressFlag_D, releaseFlag_D);

    if (shortPressFlag_A) {
        shortPressFlag_A = false;
        Event* e = new Event(EventType::APressed);
        return e;
    }
    if (longPressFlag_A) {
        longPressFlag_A = false;
        Event* e = new Event(EventType::ALongPressed);
        return e;
    }

    if (shortPressFlag_B) {
        shortPressFlag_B = false;
        Event* e = new Event(EventType::BPressed);
        return e;
    }
    if (longPressFlag_B) {
        longPressFlag_B = false;
        Event* e = new Event(EventType::BLongPressed);
        return e;
    }

    if (shortPressFlag_C) {
        shortPressFlag_C = false;
        Event* e = new Event(EventType::CPressed);
        return e;
    }
    if (longPressFlag_C) {
        longPressFlag_C = false;
        Event* e = new Event(EventType::CLongPressed);
        return e;
    }

    if (shortPressFlag_D) {
        shortPressFlag_D = false;
        Event* e = new Event(EventType::DPressed);
        return e;
    }
    if (longPressFlag_D) {
        longPressFlag_D = false;
        Event* e = new Event(EventType::DLongPressed);
        return e;
    }

    if (releaseFlag_A || releaseFlag_B || releaseFlag_C || releaseFlag_D) {
        releaseFlag_A = releaseFlag_B = releaseFlag_C = releaseFlag_D = false;
        Event* e = new Event(EventType::BtnReleased);
        return e;
    }
    return nullptr;
}

//灯光显示
void ledShow()
{
    modeID = stateMachine.getCurrentState()->getID();
    if(modeID == State1::ID)
    {
        ledTime = STATE_1_LED_TIME;
    }
    else if(modeID == State2::ID)
    {
        ledTime = STATE_2_LED_TIME;
    }
    else if(modeID == State3::ID)
    {
        ledTime = STATE_3_LED_TIME;
    }
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillisLED >= ledTime)
    {
        previousMillisLED = millis();
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
}

 musicData channel_1_chord = {
    CHANNEL_0,
    {
                {NOTE_C4, true},
                {NOTE_E4, true},
                {NOTE_G4, true},
                {NOTE_C5, false}
    },
    VELOCITY_MAX ,                 // velocity
    500,                           // BPM
    NOTE_COUNT_DEFAULT             //Note count (now is 4 -- C4,E4,G4,C5)
};

musicData channel_2_chord = {
    CHANNEL_1,
    {
                {NOTE_G6, true},
                {NOTE_E4, false},
                {NOTE_G4, false},
                {NOTE_C5, false}
    },
    VELOCITY_MAX ,                 // velocity
    500,                           // BPM
    NOTE_COUNT_DEFAULT             //Note count (now is 4 -- C4,E4,G4,C5)
};

//多轨播放
void multiTrackPlay()
{
    unsigned long currentMillis = millis();
    if(channel_1_on_off_flag)
    {
        if (currentMillis - preMillisCh_1 >= channel_1_chord.bpm)
        {
            preMillisCh_1 = currentMillis;
            synth.playChord(channel_1_chord);
        }
    }

    if(channel_2_on_off_flag)
    {
        if(currentMillis - preMillisCh_2 >= channel_2_chord.bpm)
        {
            preMillisCh_2 = currentMillis;
            synth.playChord(channel_2_chord);
        }
    }
}

void setup()
{
    //初始化串口
    Serial.begin(115200);
    //初始化LED
    pinMode(LED_BUILTIN, OUTPUT);
    initButtons();
    delay(3000);
    //初始化音序器
    synth.begin();
    //注册按钮状态
    manager->registerState(new State1());
    manager->registerState(new State2());
    manager->registerState(new State3());
    //注册错误状态
    ErrorState* errorState = new ErrorState();
    manager->registerState(errorState);
    //初始化状态机
    if(!(stateMachine.init(manager->getState(State1::ID), errorState)))
    {
        StateManager::releaseInstance();
        return ;
    }
    Serial.println("synth and state machine ready!");
}

void loop()
{
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

// // 多轨播放
// void multiTrackPlay()
// {
//     unsigned long interval = (BASIC_TIME / synth.getBpm()) / (noteType + 1);
//     unsigned long currentMillis = millis();
//     if (currentMillis - previousMillis >= interval)
//     {
//         previousMillis = currentMillis;
//         if(channel_1_on_off_flag)
//         {
//             uint8_t pitch = synth.getPitch();
//             synth.setNoteOn(CHANNEL_0,synth.getPitch(),voice);
//         }
//         if(channel_2_on_off_flag)
//         {
//             synth.setNoteOn(CHANNEL_1,synth.getPitch(),voice);
//         }
//         if(channel_3_on_off_flag)
//         {
//             synth.setNoteOn(CHANNEL_2,synth.getPitch(),voice);
//         }
//         if(channel_4_on_off_flag)
//         {
//             synth.setNoteOn(CHANNEL_3,synth.getPitch(),voice);
//         }
//         if(drum_on_off_flag)
//         {
//             if(drupCount % 4 == 0)
//             {
//                 synth.setNoteOn(CHANNEL_9,NOTE_C2,voice);
//                 synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
//             }
//             else if(drupCount % 4 == 1)
//             {
//                 synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
//             }
//             else if(drupCount % 4 == 2)
//             {
//                 synth.setNoteOn(CHANNEL_9,NOTE_D2,voice);
//                 synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
//             }
//             else if(drupCount % 4 == 3)
//             {
//                 synth.setNoteOn(CHANNEL_9,NOTE_FS2,voice);
//             }
//             drupCount++;
//         }
//     }
// }



