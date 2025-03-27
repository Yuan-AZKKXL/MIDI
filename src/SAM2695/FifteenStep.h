// ---------------------------------------------------------------------------
//
// FifteenStep.h
// A generic MIDI sequencer library for Arduino.
// // Author: Todd Treece <todd@uniontownlabs.org>
// Copyright: (c) 2015 Adafruit Industries
// License: GNU GPLv3
// // ---------------------------------------------------------------------------
#ifndef _FifteenStep_h
#define _FifteenStep_h

#include "Arduino.h"

#define DEFAULT_BPM 120
#define DEFAULT_STEPS 16
#define DEFAULT_MEMORY 512
#define MIN_TEMPO 40
#define MAX_TEMPO 240
#define MAX_STEPS 256

using MIDIcallback = void (*)(byte channel, byte command, byte arg1, byte arg2);
using StepCallback = void (*)(int current, int last);

typedef struct
{
  byte channel;
  byte pitch;
  byte velocity;
  byte step;
} FifteenStepNote;

// default values for sequence array members
const FifteenStepNote DEFAULT_NOTE = {0x0, 0x0, 0x0, 0x0};

class FifteenStep
{
public:
  FifteenStep();
  FifteenStep(int memory);
  void  begin();
  void  begin(int tempo);
  void  begin(int tempo, int steps);
  void  run();
  void  setBpm(int tempo);
  int   getBpm();
  void  increaseBpm(const uint8_t value);
  void  decreaseBpm(const uint8_t value);
  void  setSteps(int steps);
  void  setMidiHandler(MIDIcallback cb);
  void  setStepHandler(StepCallback cb);
  void  setNote(byte channel, byte pitch, byte velocity, byte step = -1);
  byte  getPosition();
  FifteenStepNote* getSequence();

private:
  int               _quantizedPosition();
  int               _greater(int first, int second);
  void              _init(int memory);
  void              _heapSort();
  void              _siftDown(int root, int bottom);
  void              _resetSequence();
  void              _loopPosition();
  void              _tick();
  void              _step();
  void              _triggerNotes();

private:
  MIDIcallback      _midiCallBack;
  StepCallback      _stepCallBack;
  FifteenStepNote*  _sequence;
  bool              _running;
  int               _sequenceSize;
  int               _tempo;
  byte              _steps;
  byte              _position;
  unsigned long     _clock;
  unsigned long     _sixteenth;
  unsigned long     _shuffle;
  unsigned long     _nextBeat;
  unsigned long     _nextClock;
};

#endif
