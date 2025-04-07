//
// Created by Administrator on 25-4-7.
//

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

#define BUTTON_A_PIN 1
#define BUTTON_B_PIN 2
#define BUTTON_C_PIN 3
#define BUTTON_D_PIN 4

extern bool shortPressFlag1;
extern bool longPressFlag1;
extern bool releaseFlag1;
extern bool shortPressFlag2;
extern bool longPressFlag2;
extern bool releaseFlag2;
extern bool shortPressFlag3;
extern bool longPressFlag3;
extern bool releaseFlag3;
extern bool shortPressFlag4;
extern bool longPressFlag4;
extern bool releaseFlag4;

void initButtons();
void detectButtonEvents(uint8_t buttonPin,  bool& shortPressFlag,  bool& longPressFlag,  bool& releaseFlag);



#endif //BUTTON_H
