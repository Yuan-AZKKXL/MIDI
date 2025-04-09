//
// Created by Administrator on 25-4-7.
//

#ifndef SOFTWARESERIAL_H
#define SOFTWARESERIAL_H


#include <Arduino.h>

class SoftwareSerial {
public:
    SoftwareSerial();
    SoftwareSerial(int rx, int tx);
    SoftwareSerial(int rx);
    void begin(int baud);
    void write(char ch);
    void print(const char *ch);
    void println(const char *ch);
    void print(char ch);
    void println(char ch);
    void print(String str);
    void println(String str);
    char available();
    int read();

    int _rx;
    int _tx;
    int _baud;
    double timeInterval;
    uint32_t timeMS;
};



#endif //SOFTWARESERIALCLASS_H
