//
// Created by Administrator on 25-4-1.
//

#ifndef SAM2695SYNTH_H
#define SAM2695SYNTH_H

#include <Arduino.h>
#include "SAM2695_Def.h"

struct Note {
    uint8_t channel;
    uint8_t pitch;
    uint8_t velocity;
};

class SAM2695Synth {
public:
    SAM2695Synth();
    void begin(HardwareSerial *serial = &Serial2, int baud = MIDI_SERIAL_BAUD_RATE,
               uint8_t RX = 42, uint8_t TX = 43);

    void setInstrument(uint8_t bank, uint8_t channel, uint8_t value);
    void setNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity);
    void setNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity);
    void setAllNotesOff(uint8_t channel);

private:
    void sendCMD(byte* cmd,int len);

private:
    HardwareSerial*   _serial;
    Note              _note;
};



#endif //SAM2695SYNTH_H
