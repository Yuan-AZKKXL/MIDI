#include <Arduino.h>
#include "Button/EmmaButton.hpp"
#include "SAM2695/SAM2695Driver.h"

EmmaButton button;
SAM2695_Driver seq = SAM2695_Driver();
int bpm = DEFAULT_BPM;
int BEAT_DURATION_MS = 60000 / (bpm * 24);
struct StepNote2 stepNote;

TaskHandle_t drum_loop_handler_B;
bool isDrumPlaying_B = false;
TaskHandle_t drum_loop_handler_D;
bool isDrumPlaying_D = false;



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

void midi2On(struct StepNote2 &stepNote)
{
    for(int i = 0; i < 10; i++)
    {
        int ms =  60000 / (stepNote.bpm * 100);
        Serial.write(0x90 | stepNote.channel);
        Serial.write(stepNote.pitch);
        Serial.write(stepNote.velocity);
        delay(ms * 100);

        Serial.write(0x90 | stepNote.channel);
        Serial.write(stepNote.pitch);
        Serial.write(stepNote.velocity);
        delay(ms * 100);

        Serial.write(0x90 | stepNote.channel);
        Serial.write(stepNote.pitch);
        Serial.write(stepNote.velocity);
        delay(ms * 100);

        Serial.write(0x90 | stepNote.channel);
        Serial.write(stepNote.pitch);
        Serial.write(stepNote.velocity);
        delay(ms * 100);

        Serial.write(0x90 | stepNote.channel);
        Serial.write(stepNote.pitch);
        Serial.write(stepNote.velocity);
        delay(ms * 100);

        stepNote.bpm += 15;
    }
}

void midi2Off(const struct StepNote2 &stepNote)
{
    Serial.write(0x80 | stepNote.channel);
    Serial.write(stepNote.pitch);
    Serial.write(0);
}

void drum_loop_proc(void *param)
{
    for (;;)
    {
        midi(9,0x9, 36, 50);
        midi(9,0x9, 42, 50);
        delay(20 * 30);
        midi(9,0x9, 42, 50);
        delay(20 * 30);
        midi(9,0x9, 38, 50);
        midi(9,0x9, 42, 50);
        delay(20 * 30);
        midi(9,0x9, 42, 50);
        delay(20 * 30);

        midi(9,0x9, 36, 50);
        midi(9,0x9, 42, 50);
        delay(20 * 30);
        midi(9,0x9, 36, 50);
        midi(9,0x9, 42, 50);
        delay(20 * 30);
        midi(9,0x9, 38, 50);
        midi(9,0x9, 42, 50);
        delay(20 * 30);
        midi(9,0x9, 42, 50);
        delay(20 * 30);
    }
}

// midi2OnTask 任务函数
void midi2OnTask(void *param) {
    StepNote2 *stepNote = (StepNote2 *)param;
    for(;;)
    {
        midi2On(*stepNote);  // 调用你的 midi2On 函数
    }
}



void setup()
{
    Serial.begin(31250);
    seq.begin();
    seq.setMidiHandler(midi);
    seq.setStepHandler(step);
    xTaskCreate(drum_loop_proc, "drumB", 1024 * 2, nullptr, 5, &drum_loop_handler_B);
    vTaskSuspend(drum_loop_handler_B);
    // 创建 midi2On 任务
    xTaskCreate(midi2OnTask, "MIDI2On_Task", 1024 * 2, nullptr, 5, &drum_loop_handler_D);
    vTaskSuspend(drum_loop_handler_D);  // 初始时挂起任务
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
        if (isDrumPlaying_B == false){
            isDrumPlaying_B = true;
            vTaskResume(drum_loop_handler_B);
        }else{
            isDrumPlaying_B = false;
            vTaskSuspend(drum_loop_handler_B);
        }
    }

    if(button.C.pressed() == BtnAct::pressed){
        bpm += 100;
        BEAT_DURATION_MS = 60000 / (bpm * 24);
    }

    // if(button.D.pressed() == BtnAct::pressed)
    // {
    //     bpm = 120;
    //     BEAT_DURATION_MS = 60000 / (bpm * 24);
    // }

    if(button.D.pressed() == BtnAct::pressed)
    {
        stepNote.channel = 5;
        stepNote.pitch = 36;
        stepNote.velocity = 127;
        stepNote.bpm = DEFAULT_BPM;
        if (isDrumPlaying_D == false)
        {
            isDrumPlaying_D = true;
            xTaskCreate(midi2OnTask, "MIDI2On_Task", 1024 * 2, &stepNote, 5, &drum_loop_handler_D);
            vTaskResume(drum_loop_handler_D);
        }
        else
        {
            isDrumPlaying_D = false;
            vTaskSuspend(drum_loop_handler_D);
        }
    }

    seq.run();
}

