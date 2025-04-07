//
// Created by Administrator on 25-4-7.
//

#include "SoftwareSerialClass.h"

SoftwareSerial::SoftwareSerial(byte rx, byte tx, unsigned long baudRate) {
    rxPin = rx;
    txPin = tx;
    bitDelay = 1000000 / baudRate;
}

void SoftwareSerial::begin() {
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    digitalWrite(txPin, HIGH);
}

void SoftwareSerial::startBit() {
    digitalWrite(txPin, LOW);
    delayMicroseconds(bitDelay);
}

void SoftwareSerial::stopBit() {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(bitDelay);
}

void SoftwareSerial::sendBit(bool bit) {
    digitalWrite(txPin, bit);
    delayMicroseconds(bitDelay);
}

bool SoftwareSerial::receiveBit() {
    delayMicroseconds(bitDelay / 2);
    bool bit = digitalRead(rxPin);
    delayMicroseconds(bitDelay / 2);
    return bit;
}

void SoftwareSerial::sendByte(byte data) {
    startBit();
    for (int i = 0; i < 8; i++) {
        sendBit(bitRead(data, i));
    }
    stopBit();
}

byte SoftwareSerial::receiveByte() {
    while (digitalRead(rxPin) == HIGH);
    delayMicroseconds(bitDelay / 2);
    byte data = 0;
    for (int i = 0; i < 8; i++) {
        bitWrite(data, i, receiveBit());
    }
    delayMicroseconds(bitDelay);
    return data;
}

// #include "SoftwareSerialClass.h"
// SoftwareSerial mySerial(2, 3, 9600);
// void setup() {
//     Serial.begin(9600);
//     mySerial.begin();
// }
// void loop() {
//     if (Serial.available()) {
//         byte data = Serial.read();
//         mySerial.sendByte(data);
//     }
//     if (mySerial.receiveByte()) {
//         byte data = mySerial.receiveByte();
//         Serial.write(data);
//     }
// }
