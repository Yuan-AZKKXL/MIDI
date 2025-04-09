//
// Created by Administrator on 25-4-7.
//

#include "SoftwareSerialClass.h"

SoftwareSerial::SoftwareSerial() {
}

SoftwareSerial::SoftwareSerial(int rx, int tx) {
    _rx = rx;
    _tx = tx;
    //TX mode
    if (rx < 0) {
        pinMode(_tx, OUTPUT);
        digitalWrite(_tx, HIGH);
    } else {
        pinMode(_rx, INPUT_PULLUP);
        pinMode(_tx, OUTPUT);
        digitalWrite(_tx, HIGH);
    }
}

SoftwareSerial::SoftwareSerial(int rx){
    _rx = rx;
    pinMode(_rx, INPUT_PULLUP);
}

void SoftwareSerial::begin(int baud) {
    _baud = baud;
    timeInterval = (1.0 / (float)baud);
    timeMS = (uint32_t)(timeInterval * 1000000);
}

void SoftwareSerial::write(char ch) {
    digitalWrite(_tx, LOW);
    delayMicroseconds(timeMS);
    for (int a = 0; a < 8; a++) {
        if (ch & (1 << a)) {
            digitalWrite(_tx, HIGH);
        } else {
            digitalWrite(_tx, LOW);
        }
        delayMicroseconds(timeMS);
    }
    digitalWrite(_tx, HIGH);
    delayMicroseconds(timeMS);
}

void SoftwareSerial::print(const char *ch) {
    while (*ch) {
        write(*ch);
        ch++;
    }
}

void SoftwareSerial::println(const char *ch) {
    while (*ch) {
        write(*ch);
        ch++;
    }
    write('\r');
    write('\n');
}

void SoftwareSerial::print(char ch){
    write(ch);
}

void SoftwareSerial::println(char ch){
    write(ch);
    write('\r');
    write('\n');
}

void SoftwareSerial::print(String str){
    print(str.c_str());
}

void SoftwareSerial::println(String str){
    println(str.c_str());
}

char SoftwareSerial::available() {
    int x = digitalRead(_rx);
    if (x) {
        return 0;
    } else {
        return 1;
    }
}

int SoftwareSerial::read() {

    int ch = 0;
    for (int a = 0; a < 9; a++) {
        if (digitalRead(_rx)) {
            ch |= (1 << a);
        }
        delayMicroseconds(timeMS);
    }
    digitalWrite(_rx, HIGH);
    return ch >> 1;
}
