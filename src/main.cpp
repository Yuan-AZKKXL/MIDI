#include <Arduino.h>
#include "Event/AuditionMode.h"
#include "SAM2695/SAM2695Synth.h"
#include "Button/Button.h"
#include "Event/BpmMode.h"
#include "Event/TrackMode.h"
#include "Event/ErrorState.h"

//定义不同模式下的指示灯
#define STATE_1_LED_TIME 800
#define STATE_2_LED_TIME 400
#define STATE_3_LED_TIME 200
#define LED_PIN LED_BUILTIN

//创建按钮所需要的结构体
BtnState btnA = {HIGH, HIGH, 0, 0, false};
BtnState btnB = {HIGH, HIGH, 0, 0, false};
BtnState btnC = {HIGH, HIGH, 0, 0, false};
BtnState btnD = {HIGH, HIGH, 0, 0, false};
//定义按钮和状态机事件的数组
ButtonFlags buttonFlags[] = {
    {shortPressFlag_A, longPressFlag_A, releaseFlag_A, EventType::APressed, EventType::ALongPressed},
    {shortPressFlag_B, longPressFlag_B, releaseFlag_B, EventType::BPressed, EventType::BLongPressed},
    {shortPressFlag_C, longPressFlag_C, releaseFlag_C, EventType::CPressed, EventType::CLongPressed},
    {shortPressFlag_D, longPressFlag_D, releaseFlag_D, EventType::DPressed, EventType::DLongPressed}
};

//轨道和弦所需结构体示例
const musicData channel_1_chord =
{
    CHANNEL_9,
    {
        {NOTE_C2, true},
        {NOTE_FS2, true},
    },
    VELOCITY_DEFAULT ,
    synth.bpmToMs(BPM_DEFAULT),
    0,
};

const musicData channel_2_chord =
{
CHANNEL_9,
{
                {NOTE_FS2, true},
    },
    VELOCITY_DEFAULT ,
    synth.bpmToMs(BPM_DEFAULT),
    1
};

const musicData channel_3_chord =
{
CHANNEL_9,
{
                    {NOTE_D2, true},
                    {NOTE_FS2, true},
        },
        VELOCITY_DEFAULT ,
    synth.bpmToMs(BPM_DEFAULT),
    2
};
const musicData channel_4_chord =
{
CHANNEL_9,
{
                    {NOTE_FS2, true},
        },
        VELOCITY_DEFAULT ,
        synth.bpmToMs(BPM_DEFAULT),
    3
};
musicData track2[] = {channel_1_chord, channel_2_chord, channel_3_chord, channel_4_chord};

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
unsigned long preMillisCh_drup = 0;                 // 鼓点轨道上一次发送MIDI信号的时间
int noteType = QUATER_NOTE;                         // 音符类型选择，0（四分音符）、1（八分音符）、2（十六音符）
int beatsPerBar = BEATS_BAR_DEFAULT;                // 每小节拍数，可以是2、3或4
uint8_t drupCount = 0;                              //鼓点轨道播放计数器

//指示灯状态
uint8_t  modeID = AuditionMode::ID;                       //模式ID
int ledTime = STATE_1_LED_TIME;                     //LED反转时间，500ms
unsigned long previousMillisLED = 0;                //记录上一次灯的时间

// 事件数组的大小
#define EVENT_ARR_SIZE  3
Event eventArray[EVENT_ARR_SIZE];

// //获取下一个事件
Event* getNextEvent()
{
    // detectButtonEvents(BUTTON_A_PIN, btnA, act);
    detectButtonEvents(BUTTON_A_PIN, btnA, shortPressFlag_A, longPressFlag_A, releaseFlag_A);
    detectButtonEvents(BUTTON_B_PIN, btnB, shortPressFlag_B, longPressFlag_B, releaseFlag_B);
    detectButtonEvents(BUTTON_C_PIN, btnC, shortPressFlag_C, longPressFlag_C, releaseFlag_C);
    detectButtonEvents(BUTTON_D_PIN, btnD, shortPressFlag_D, longPressFlag_D, releaseFlag_D);

    // 检查短按和长按标志
    for (const auto& flags : buttonFlags) {
        if (flags.shortPress) {
            flags.shortPress = false;
            Serial.println("short press");
            for (int i = 0; i < EVENT_ARR_SIZE; i++) {
                if (!eventArray[i].isInUse()) {
                    eventArray[i].setType(flags.shortPressType);
                    eventArray[i].setInUse(true);
                    return &eventArray[i];
                }
            }

        }
        if (flags.longPress) {
            flags.longPress = false;
            for (int i = 0; i < EVENT_ARR_SIZE; i++) {
                if (!eventArray[i].isInUse()) {
                    eventArray[i].setType(flags.longPressType);
                    eventArray[i].setInUse(true);
                    return &eventArray[i];
                }
            }
            // return eventPool.getEvent(flags.longPressType);
        }
    }

    bool anyReleased = false;
    for (auto& flags : buttonFlags) {
        if (flags.release) {
            anyReleased = true;
            flags.release = false;
        }
    }

    if (anyReleased) {
        for (int i = 0; i < EVENT_ARR_SIZE; i++) {
            if (!eventArray[i].isInUse()) {
                eventArray[i].setType(EventType::BtnReleased);
                eventArray[i].setInUse(true);
                return &eventArray[i];
            }
        }
        // return eventPool.getEvent(EventType::BtnReleased);
    }

    return nullptr;
}

//回收事件
void recycleEvent(Event* event) {
    if (event) {
        event->setType(EventType::None);
        event->setInUse(false);  // 标记事件为未使用
    }
}

//灯光显示
void ledShow()
{
    modeID = stateMachine.getCurrentState()->getID();
    if(modeID == AuditionMode::ID)
    {
        ledTime = STATE_1_LED_TIME;
    }
    else if(modeID == BpmMode::ID)
    {
        ledTime = STATE_2_LED_TIME;
    }
    else if(modeID == TrackMode::ID)
    {
        ledTime = STATE_3_LED_TIME;
    }
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillisLED >= ledTime)
    {
        previousMillisLED = millis();
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}

//多轨和弦播放
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

    if(channel_3_on_off_flag)
    {
        if(currentMillis - preMillisCh_3 >= channel_3_chord.bpm)
        {
            preMillisCh_3 = currentMillis;
            synth.playChord(channel_3_chord);
        }
    }

    if(channel_4_on_off_flag)
    {
        if(currentMillis - preMillisCh_4 >= channel_4_chord.bpm)
        {
            preMillisCh_4 = currentMillis;
            synth.playChord(channel_4_chord);
        }
    }

    unsigned long interval = (BASIC_TIME / synth.getBpm()) / (noteType + 1);
    if (currentMillis - preMillisCh_drup >= interval)
    {
        preMillisCh_drup = currentMillis;
        if(drum_on_off_flag)
        {
            if(track2[drupCount].index == drupCount)
            {
                synth.playChord(track2[drupCount]);
            }
            drupCount = (drupCount+1) % (sizeof(track2)/sizeof(track2[0]));
        }
    }
}

void setup()
{
    //初始化串口
    Serial.begin(115200);
    //初始化LED
    pinMode(LED_PIN, OUTPUT);
    initButtons();
    delay(3000);
    //初始化音序器
    synth.begin();
    //注册按钮状态
    manager->registerState(new AuditionMode());
    manager->registerState(new BpmMode());
    manager->registerState(new TrackMode());
    //注册错误状态
    ErrorState* errorState = new ErrorState();
    manager->registerState(errorState);
    //初始化状态机
    if(!(stateMachine.init(manager->getState(AuditionMode::ID), errorState)))
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
        recycleEvent(event);
    }
    multiTrackPlay();
    ledShow();
}





