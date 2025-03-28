// ---------------------------------------------------------------------------
//
// SAM2695Driver.h
// A generic MIDI sequencer library for Arduino.
// Copyright: (c) 2015 Adafruit Industries
// License: GNU GPLv3
// // ---------------------------------------------------------------------------
#ifndef _FifteenStep_h
#define _FifteenStep_h

#include "Arduino.h"


/*------------------------------------------- Defines begin -------------------------------------------*/
#define DEFAULT_BPM 120
#define DEFAULT_STEPS 16
#define DEFAULT_MEMORY 512
#define MIN_TEMPO 40
#define MAX_TEMPO 240
#define MAX_STEPS 256
/*------------------------------------------- Defines end -------------------------------------------*/

/*------------------------------------------- Structs begin -------------------------------------------*/
typedef struct
{
  byte channel;
  byte pitch;
  byte velocity;
  byte step;
} StepNote;

//! add new struct for test
struct StepNote2
{
  byte channel;
  byte pitch;
  byte velocity;
  byte bpm;
};

// default values for sequence array members
const StepNote DEFAULT_NOTE = {0x0, 0x0, 0x0, 0x0};
/*------------------------------------------- Structs end -------------------------------------------*/

/*------------------------------------------- callback function type -------------------------------------------*/
using MIDIcallback = void (*)(byte channel, byte command, byte arg1, byte arg2);
using StepCallback = void (*)(int current, int last);
//! add new callback function type for test
using MIDIcallBack2 = void(*)(struct StepNote2& note);
/**------------------------------------------- callback function end -------------------------------------------*/


class SAM2695_Driver
{
public:
  SAM2695_Driver(int memory = DEFAULT_MEMORY);
  void  begin(int bpms = DEFAULT_BPM, int steps = DEFAULT_STEPS);
  void  run();
  void  setBpm(int tempo);
  int   getBpm();
  void  setSteps(int steps);
  void  setMidiHandler(MIDIcallback cb);
  void  setStepHandler(StepCallback cb);
  void  setNote(byte channel, byte pitch, byte velocity, byte step = -1);
  byte  getPosition();
  StepNote* getStepNote();

  //todo add test
  void  setMidiHandler2(MIDIcallBack2 cb);

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
  StepNote*  _sequence;

  //todo add test
  MIDIcallBack2     _midiCallBack2;

  bool              _running;
  int               _sequenceSize;
  int               _bpm;
  byte              _steps;
  byte              _position;
  unsigned long     _clock;
  unsigned long     _sixteenth;
  unsigned long     _shuffle;
  unsigned long     _nextBeat;
  unsigned long     _nextClock;
};

#endif
