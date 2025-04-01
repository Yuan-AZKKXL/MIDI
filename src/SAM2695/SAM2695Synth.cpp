//
// Created by Administrator on 25-4-1.
//

#include "SAM2695Synth.h"

SAM2695Synth::SAM2695Synth()
    :_serial(nullptr)
{

}

void SAM2695Synth::begin(HardwareSerial* serial, int baud, uint8_t RX, uint8_t TX)
{
    _serial = serial;
    _serial->begin(baud, SERIAL_8N1, RX, TX);
}

void SAM2695Synth::setInstrument(uint8_t bank, uint8_t channel, uint8_t value)
{
    uint8_t CMD_CONTROL_CHANGE_1[] = {
        (uint8_t)(MIDI_CMD_CONTROL_CHANGE | (channel & 0x0f)), 0x00, bank};

    sendCMD(CMD_CONTROL_CHANGE_1, sizeof(CMD_CONTROL_CHANGE_1));

    uint8_t CMD_PROGRAM_CHANGE_2[] = {
        (uint8_t)(MIDI_CMD_PROGRAM_CHANGE | (channel & 0x0f)), value};
    sendCMD(CMD_PROGRAM_CHANGE_2, sizeof(CMD_PROGRAM_CHANGE_2));
}

void SAM2695Synth::setNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    uint8_t CMD_NOTE_ON[] = {(uint8_t)(MIDI_COMMAND_ON | (channel & 0x0f)),
                             pitch, velocity};
    sendCMD(CMD_NOTE_ON, sizeof(CMD_NOTE_ON));
}

void SAM2695Synth::setNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    uint8_t CMD_NOTE_OFF[] = {(uint8_t)(MIDI_COMMAND_OFF | (channel & 0x0f)),
                              pitch, 0x00};
    sendCMD(CMD_NOTE_OFF, sizeof(CMD_NOTE_OFF));
}

void SAM2695Synth::setAllNotesOff(uint8_t channel)
{
    uint8_t CMD_CONTROL_CHANGE[] = {
        (uint8_t)(MIDI_CMD_CONTROL_CHANGE | (channel & 0x0f)), 0x7b, 0x00};
    sendCMD(CMD_CONTROL_CHANGE, sizeof(CMD_CONTROL_CHANGE));
}


void SAM2695Synth::sendCMD(byte* cmd, int len)
{
    _serial->write(cmd, len);
}
