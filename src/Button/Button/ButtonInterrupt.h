//
// Created by Administrator on 25-4-2.
//

#ifndef BUTTONINTERRUPT_H
#define BUTTONINTERRUPT_H

#include <Arduino.h>

#define EMMA_BTN_A_PIN 1
#define EMMA_BTN_B_PIN 2
#define EMMA_BTN_C_PIN 3
#define EMMA_BTN_D_PIN 4

//短按标志位
extern volatile bool APressedFlag;
extern volatile bool BPressedFlag;
extern volatile bool CPressedFlag;
extern volatile bool DPressedFlag;
//长按标志位
extern volatile bool ALongPressed;
extern volatile bool BLongPressed;
extern volatile bool CLongPressed;
extern volatile bool DLongPressed;

extern unsigned long firstv;
extern unsigned long secondv;


void buttonAInterrupt();
void buttonBInterrupt();
void buttonCInterrupt();
void buttonDInterrupt();


#endif //BUTTONINTERRUPT_H
