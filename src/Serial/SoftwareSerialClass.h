//
// Created by Administrator on 25-4-7.
//

#ifndef SOFTWARESERIALCLASS_H
#define SOFTWARESERIALCLASS_H



#include <Arduino.h>
class SoftwareSerial {
public:
    SoftwareSerial(byte rx, byte tx, unsigned long baudRate);
    void begin();
    void sendByte(byte data);
    byte receiveByte();
private:
    byte rxPin;
    byte txPin;
    unsigned long bitDelay;
    void startBit();
    void stopBit();
    void sendBit(bool bit);
    bool receiveBit();
};



#endif //SOFTWARESERIALCLASS_H
