#include <Arduino.h>
#include "Button/EmmaButton.hpp"
#include "SAM2695/FifteenStep.h"

EmmaButton button;
FifteenStep seq = FifteenStep();
TaskHandle_t drum_loop_handler;
bool isDrumPlaying = false;
int bpm = DEFAULT_BPM;
int BEAT_DURATION_MS = 60000 / (bpm * 24);




void step(int current, int last) {
    if(current % 2 == 0)
        digitalWrite(13, HIGH);
    else
        digitalWrite(13, LOW);
}

void midi(byte channel, byte command, byte arg1, byte arg2)
{
    if(command < 128) {
        command <<= 4;
        command |= channel;
    }

    Serial.write(command);
    Serial.write(arg1);
    Serial.write(arg2);
}


void drum_loop_proc(void *param)
{
    for (;;)
    {
        midi(9,0x9, 36, 50);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);
        midi(9,0x9, 38, 50);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);

        midi(9,0x9, 36, 50);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);
        midi(9,0x9, 36, 50);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);
        midi(9,0x9, 38, 50);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);
        midi(9,0x9, 42, 50);
        delay(BEAT_DURATION_MS * 100);
    }
}




void setup()
{
    Serial.begin(31250);
    seq.begin();
    seq.setMidiHandler(midi);
    seq.setStepHandler(step);
    xTaskCreate(drum_loop_proc, "drum", 1024 * 2, nullptr, 5, &drum_loop_handler);
    vTaskSuspend(drum_loop_handler);
}

void loop()
{
    if (button.A.pressed() == BtnAct::pressed){
        midi(0, 0x9, 77, 127);
        // seq.setNote(0, 77, 127);
    }else if (button.A.released() == BtnAct::released){
        midi(0, 0x8, 77, 0);
        // seq.setNote(0, 77, 0);
    }

    if(button.B.pressed() == BtnAct::pressed){
        if (isDrumPlaying == false){
            isDrumPlaying = true;
            vTaskResume(drum_loop_handler);
        }else{
            isDrumPlaying = false;
            vTaskSuspend(drum_loop_handler);
        }
    }

    if(button.C.pressed() == BtnAct::pressed){
        bpm += 100;
        BEAT_DURATION_MS = 60000 / (bpm * 24);
    }

    if(button.D.pressed() == BtnAct::pressed)
    {
        bpm = 120;
        BEAT_DURATION_MS = 60000 / (bpm * 24);
    }
    seq.run();
}

