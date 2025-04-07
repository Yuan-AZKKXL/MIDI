//
// Created by Administrator on 25-4-7.
//

#include "Button.h"

// 消抖时间（毫秒）
const unsigned long debounceDelay = 50;
// 长按时间（毫秒）
const unsigned long longPressTime = 1000;

// 事件标志位
bool shortPressFlag1 = false;
bool longPressFlag1 = false;
bool releaseFlag1 = false;
bool shortPressFlag2 = false;
bool longPressFlag2 = false;
bool releaseFlag2 = false;
bool shortPressFlag3 = false;
bool longPressFlag3 = false;
bool releaseFlag3 = false;
bool shortPressFlag4 = false;
bool longPressFlag4 = false;
bool releaseFlag4 = false;

void initButtons()
{
    pinMode(BUTTON_A_PIN,INPUT_PULLUP);
    pinMode(BUTTON_B_PIN,INPUT_PULLUP);
    pinMode(BUTTON_C_PIN,INPUT_PULLUP);
    pinMode(BUTTON_D_PIN,INPUT_PULLUP);
}

// 封装按键检测函数
void detectButtonEvents(uint8_t buttonPin, bool& shortPressFlag, bool& longPressFlag, bool& releaseFlag) {
    static int buttonState;
    static int lastButtonState = HIGH;
    static unsigned long lastDebounceTime = 0;
    static unsigned long pressStartTime = 0;
    static bool longPressTriggered = false;

    // 读取按键状态
    int reading = digitalRead(buttonPin);

    // 检测按键状态是否改变
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    // 消抖处理
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == LOW) {
                // 按键按下
                pressStartTime = millis();
                longPressTriggered = false;
            } else {
                // 按键释放
                unsigned long pressDuration = millis() - pressStartTime;
                if (!longPressTriggered && pressDuration < longPressTime) {
                    // 短按事件
                    shortPressFlag = true;
                }
                releaseFlag = true;
            }
        }
    }

    // 检测长按事件
    if (buttonState == LOW && (millis() - pressStartTime) >= longPressTime) {
        if (!longPressTriggered) {
            longPressFlag = true;
            longPressTriggered = true;
        }
    }

    // 更新上一次的按键状态
    lastButtonState = reading;
}