#include <Arduino.h>
#include "Button/EmmaButton.hpp"
#include "SAM2695/SAM2695_Def.h"
#include "Event/ButtonState.hpp"
#include "SAM2695/SAM2695Synth.h"

//creat object of EmmaButton and SAM2695_Driver
EmmaButton button;
SAM2695Synth seq = SAM2695Synth();


void setup()
{
    Serial.begin(USB_SERIAL_BAUD_RATE);
    seq.begin();
    seq.setInstrument(0, 0, BANK1_Piano1);
    delay(2000);
    Serial.println("EmmaButton and SAM2695_Driver are ready!");
}

void loop()
{
    if (button.A.pressed() == EventType::Pressed)
    {
        // @param4 volume 0-127
        seq.setNoteOn(CHANNEL_0, BANK0_BlownBottle, 127);
        Serial.println("Button pressed!");
    }
    if (button.A.released() == EventType::Released)
    {
        // @param4 volume 0-127
        seq.setNoteOff(CHANNEL_0, BANK0_BlownBottle, 0);
        Serial.println("Button released!");
    }
}

