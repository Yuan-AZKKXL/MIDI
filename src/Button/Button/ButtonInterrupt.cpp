//
// Created by Administrator on 25-4-2.
//

#include "ButtonInterrupt.h"

// 按钮和中断相关的变量
volatile bool APressedFlag = false;
volatile bool BPressedFlag = false;
volatile bool CPressedFlag = false;
volatile bool DPressedFlag = false;
volatile bool ALongPressed = false;
volatile bool BLongPressed = false;
volatile bool CLongPressed = false;
volatile bool DLongPressed = false;

unsigned long firstv = 0;
unsigned long secondv = 0;

unsigned long lastDebounceTimeA = 0;  // 按钮 A 上次去抖动的时间
unsigned long lastDebounceTimeB = 0;  // 按钮 B 上次去抖动的时间
unsigned long lastDebounceTimeC = 0;  // 按钮 C 上次去抖动的时间
unsigned long lastDebounceTimeD = 0;  // 按钮 D 上次去抖动的时间

unsigned long buttonPressStartTime = 0;
const unsigned int debounceDelay = 50;  // 去抖动的延时，单位毫秒
const unsigned int longPressTime = 2000;  // 长按阈值，单位毫秒

bool flag = false;


void buttonAInterrupt() {
    // 只有在消抖延时后才处理按键
    if ((millis() - lastDebounceTimeA) > debounceDelay)
    {
        // 如果已经过了去抖动延时
        lastDebounceTimeA = millis();
        firstv = millis();
        if (digitalRead(EMMA_BTN_A_PIN) == LOW)
            APressedFlag = true;
    }
}

void buttonBInterrupt() {
    if ((millis() - lastDebounceTimeB) > debounceDelay)
    {
        // 如果已经过了去抖动延时
        lastDebounceTimeB = millis();
        if (digitalRead(EMMA_BTN_B_PIN) == LOW)
            BPressedFlag = true;
    }
}

void buttonCInterrupt() {
    if ((millis() - lastDebounceTimeC) > debounceDelay)
    {
        // 如果已经过了去抖动延时
        lastDebounceTimeC = millis();
        if (digitalRead(EMMA_BTN_C_PIN) == LOW)
            CPressedFlag = true;
    }
}

void buttonDInterrupt() {
    if ((millis() - lastDebounceTimeD) > debounceDelay)
    {
        // 如果已经过了去抖动延时
        lastDebounceTimeD = millis();
        if (digitalRead(EMMA_BTN_D_PIN) == LOW)
            DPressedFlag = true;
    }
}

