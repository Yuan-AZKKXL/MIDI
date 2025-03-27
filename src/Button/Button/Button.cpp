/*
	Button - a small library for Arduino to handle button debouncing
	
	MIT licensed.
*/

#include "Button.h"
#include <Arduino.h>

__Button::__Button(uint8_t pin, uint16_t debounce_ms)
:  _pin(pin)
,  _delay(debounce_ms)
,  _state(HIGH)
,  _ignore_until(0)
,  _has_changed(false)
,  pressedTime_(0)
,  longPressThreshold_(700)
,  longPressFlag_(false)
{
    pinMode(pin, INPUT_PULLUP);
}

void __Button::begin(){
	pinMode(_pin, INPUT_PULLUP);
}

// 
// public methods
// 

bool __Button::read(){
	// ignore pin changes until after this delay time
	if (_ignore_until > millis()){
		// ignore any changes during this period
	}
	
	// pin has changed 
	else if (digitalRead(_pin) != _state){
		_ignore_until = millis() + _delay;
		_state = !_state;
		_has_changed = true;
	}
	
	return _state;
}

// has the button been toggled from on -> off, or vice versa
BtnAct __Button::toggled(){
	if (has_changed()) {
		return BtnAct::toggled;
	}
	return BtnAct::none;
}

// mostly internal, tells you if a button has changed after calling the read() function
bool __Button::has_changed(){
	if (_has_changed){
		_has_changed = false;
		return true;
	}
	return false;
}

// has the button gone from off -> on
BtnAct __Button::pressed(){
	if (read() == LOW && has_changed()) {
		return BtnAct::pressed;
	}
	return BtnAct::none;
}

// has the button gone from on -> off
BtnAct __Button::released(){
	if (read() == HIGH && has_changed()) {
		return BtnAct::released;
	}
	return BtnAct::none;
}

// has the button detect long press
BtnAct __Button::longPressed(){
	// If the button is pressed, check if it has been pressed for longer than the threshold
	if (read() == PRESSED) {
		if (pressedTime_ == 0) {
			// Record the time when the button is first pressed
			pressedTime_ = millis();
		}
		// Check if the button has been pressed long enough
		if (millis() - pressedTime_ >= longPressThreshold_ ) {
			if(longPressFlag_ == false){
				longPressFlag_ = true;
				return BtnAct::longPressed;  // Long press detected
			}
			return BtnAct::none;  // Long press detected, but already reported
		}
	} else {
		// Reset the pressed_time when the button is released
		if(longPressFlag_ ){
			longPressFlag_ = false;
		}
		pressedTime_ = 0;
	}
	return BtnAct::none;   // No long press
}

