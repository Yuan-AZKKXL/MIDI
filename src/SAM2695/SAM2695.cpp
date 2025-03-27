
#include "Arduino.h"
#include "SAM2695Driver.h"



SAM2695_Driver::SAM2695_Driver()
{
  _init(DEFAULT_MEMORY);
}

SAM2695_Driver::SAM2695_Driver(int memory)
{
  _init(memory);
}

void SAM2695_Driver::begin()
{
  begin(DEFAULT_BPM, DEFAULT_STEPS);
}

void SAM2695_Driver::begin(int tempo)
{
  begin(tempo, DEFAULT_STEPS);
}

void SAM2695_Driver::begin(int tempo, int steps)
{
  setBpm(tempo);
  setSteps(steps);
}

// 开始运行
void SAM2695_Driver::run()
{

  if(! _running)
    return;

  // what's the time?
  unsigned long now = millis();
  // it's time to get ill.

  // send clock
  if(now >= _nextClock) {
    _tick();
    _nextClock = now + _clock;
  }

  // only step if it's time
  if(now < _nextBeat)
    return;

  // advance and send notes
  _step();

  // add shuffle offset to next beat if needed
  if((_position % 2) == 0)
    _nextBeat = now + _sixteenth + _shuffle;
  else
    _nextBeat = now + _sixteenth - _shuffle;

}

// 设置节拍
void SAM2695_Driver::setBpm(int tempo)
{
  if(tempo < MIN_TEMPO)
    tempo = MIN_TEMPO;
  if(tempo > MAX_TEMPO)
    tempo = MAX_TEMPO;
  _tempo = tempo;
}

int SAM2695_Driver::getBpm()
{
  return _tempo;
}

// 设置 每一步
void SAM2695_Driver::setSteps(int steps)
{
  if(steps > MAX_STEPS)
    steps = MAX_STEPS;
  _steps = steps;

  // loop through the sequence and clear notes past the current step
  for(int i=0; i < _sequenceSize; ++i)
  {
    // reset any steps that are over the current step count
    if(_sequence[i].step >= _steps)
      _sequence[i] = DEFAULT_NOTE;
  }
}

// 增加节拍
void SAM2695_Driver::increaseBpm(const uint8_t value)
{
  setBpm(_tempo + value);
}

// 减少节拍
void SAM2695_Driver::decreaseBpm(const uint8_t value)
{
  setBpm(_tempo - value);
}

//设置回调方法
void SAM2695_Driver::setMidiHandler(MIDIcallback cb)
{
  _midiCallBack = cb;
}

//设置回调方法
void SAM2695_Driver::setStepHandler(StepCallback cb)
{
  _stepCallBack = cb;
}

//设置一个音符
void SAM2695_Driver::setNote(byte channel, byte pitch, byte velocity, byte step)
{
  if(! _running)
    return;
  int position;


  bool added = false;

  if(step == -1)
    position = _quantizedPosition();
  else
    position = step;

  for(int i = _sequenceSize - 1; i >= 0; i--)
  {

    // used by another pitch, keep going
    if(_sequence[i].pitch > 0 && _sequence[i].pitch != pitch)
      continue;

    // used by another step, keep going
    if(_sequence[i].step != position && _sequence[i].pitch != 0)
      continue;

    // used by another channel, keep going
    if(_sequence[i].channel != channel && _sequence[i].pitch != 0)
      continue;

    // matches the sent step, pitch & channel
    if(_sequence[i].pitch == pitch && _sequence[i].step == position && _sequence[i].channel == channel)
    {

      if(velocity > 0 && _sequence[i].velocity > 0) {
        _sequence[i] = DEFAULT_NOTE;
        added = true;
        continue;
      } else if(velocity == 0 && _sequence[i].velocity == 0) {
        _sequence[i] = DEFAULT_NOTE;
        added = true;
        continue;
      }

    }

    // use the free slot
    if(_sequence[i].pitch == 0 && _sequence[i].step == 0 && _sequence[i].channel == 0 && !added)
    {

      _sequence[i].channel = channel;
      _sequence[i].pitch = pitch;
      _sequence[i].velocity = velocity;
      _sequence[i].step = position;

      added = true;
    }
  }
  _heapSort();
}

// getSequence
//
// Returns a pointer to the current sequence
FifteenStepNote* SAM2695_Driver::getSequence()
{
  return _sequence;
}

// getPosition
//
// Returns the closest 16th note to the
// present time. This is used to see where to
// save the new note.
byte SAM2695_Driver::getPosition()
{
  return _quantizedPosition();
}


void SAM2695_Driver::_init(int memory)
{

  _running = true;
  _nextBeat = 0;
  _nextClock = 0;
  _position = 0;
  _sequenceSize = memory / sizeof(FifteenStepNote);
  _sequence = new FifteenStepNote[_sequenceSize];

  // set up default notes
  _resetSequence();

}

// _resetSequence
//
// Sets sequence to default state
void SAM2695_Driver::_resetSequence()
{
  // set sequence to default note value
  for(int i=0; i < _sequenceSize; ++i)
    _sequence[i] = DEFAULT_NOTE;
}

// _quantizedPosition
//
// Returns the closest 16th note to the
// present time. This is used to see where to
// save the new note.
int SAM2695_Driver::_quantizedPosition()
{

  if(_shuffle > 0)
    return _position;

  // what's the time?
  unsigned long now = millis();

  // calculate value of 32nd note
  unsigned long thirty_second = _sixteenth / 2;

  // use current position if below middle point
  if(now <= (_nextBeat - thirty_second))
    return _position;

  // return first step if the next step
  // is past the step count
  if((_position + 1) >= _steps)
    return 0;

  // return next step
  return _position + 1;

}

// _step
//
// Moves _position forward by one step, calls the
// step callback with the current & last step position,
// and triggers any notes at the current position.
void SAM2695_Driver::_step()
{

  // save the last position so we
  // can provide it to the callback
  int last = _position;

  // increment the position
  _position++;

  // start over if we've reached the end
  if(_position >= _steps)
    _position = 0;

  // tell the callback where we are
  // if it has been set by the sketch
  if(_stepCallBack)
    _stepCallBack(_position, last);

  // trigger next set of notes
  _triggerNotes();

}

// _tick
//
// Calls the user defined MIDI callback with
// the midi clock message
void SAM2695_Driver::_tick()
{

  // bail if the midi callback isn't set
  if(! _midiCallBack)
    return;

  // tick
  _midiCallBack(0x0, 0xF8, 0x0, 0x0);

}

// _loopPosition
//
// Calls the user defined MIDI callback with
// the position of playback
void SAM2695_Driver::_loopPosition()
{

  // bail if the midi callback isn't set
  if(! _midiCallBack)
    return;

  // send position
  _midiCallBack(0x0, 0xF2, 0x0, _position);

}

// _heapSort
//
// Sort the sequence based on the heapsort algorithm
void SAM2695_Driver::_heapSort()
{

  int i;
  FifteenStepNote tmp;

  for(i = _sequenceSize / 2; i >= 0; i--)
    _siftDown(i, _sequenceSize - 1);

  for(i = _sequenceSize - 1; i >= 1; i--)
  {

    tmp = _sequence[0];
    _sequence[0] = _sequence[i];
    _sequence[i] = tmp;

    _siftDown(0, i - 1);

  }

}

// _siftDown
//
// Used by heapsort to shift note positions
void SAM2695_Driver::_siftDown(int root, int bottom)
{

  int max = root * 2 + 1;

  if(max < bottom)
    max = _greater(max, max + 1) == max ? max : max + 1;
  else if(max > bottom)
    return;

  if(_greater(root, max) == root || _greater(root, max) == -1)
    return;

  FifteenStepNote tmp = _sequence[root];
  _sequence[root] = _sequence[max];
  _sequence[max] = tmp;

  _siftDown(max, bottom);

}

// _greater
//
// Used by heapsort to compare two notes so we
// know where they should be placed in the sorted
// array. Will return -1 if they are equal
int SAM2695_Driver::_greater(int first, int second)
{

  if(_sequence[first].velocity > _sequence[second].velocity)
    return first;
  else if(_sequence[second].velocity > _sequence[first].velocity)
    return second;

  if(_sequence[first].pitch > _sequence[second].pitch)
    return first;
  else if(_sequence[second].pitch > _sequence[first].pitch)
    return second;

  if(_sequence[first].step > _sequence[second].step)
    return first;
  else if(_sequence[second].step > _sequence[first].step)
    return second;

  if(_sequence[first].channel > _sequence[second].channel)
    return first;
  else if(_sequence[second].channel > _sequence[first].channel)
    return second;

  return - 1;

}

// _triggerNotes
//
// Calls the user defined MIDI callback with
// all of the note on and off messages at the
void SAM2695_Driver::_triggerNotes()
{
  // bail if the midi callback isn't set
  if(! _midiCallBack)
    return;
  // loop through the sequence again and trigger note ons at the current position
  for(int i=0; i < _sequenceSize; ++i)
  {

    // ignore if it's not the current position
    if(_sequence[i].step != _position)
      continue;

    // if this position is in the default state, ignore it
    if(_sequence[i].pitch == 0 && _sequence[i].velocity == 0 && _sequence[i].step == 0)
      continue;

    // send note on values to callback
    _midiCallBack(
      _sequence[i].channel,
      _sequence[i].velocity > 0 ? 0x9 : 0x8,
      _sequence[i].pitch,
      _sequence[i].velocity
    );
  }
}
