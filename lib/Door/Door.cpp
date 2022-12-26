#include "Door.h"
#include "helper.h"

Door::Door() :
  _currentState{Closed},
  _lastState{Closed},
  _movementTimer{Ticker{}}
{}

void Door::begin() {
  pinMode(RELAY, OUTPUT);
  pinMode(SENSOR, INPUT);
  _lastState = _currentState = _getSensor();
}

Door::State Door::get() {
  _lastState = _currentState;
  return _currentState;
}

void Door::setChangeCallback(callback_t cb) {
  _changeCallback = cb;
}

void Door::set(State state) {
  // Ignore if door is in requested state or moving, or request is for transition state
  if (_currentState == state || _moving() || state > Open) return;

  // Momentarily activate relay to trigger door opener
  digitalWrite(RELAY, HIGH);
  delay(PULSE_MS);
  digitalWrite(RELAY, LOW);

  if (_currentState == Open) {
    // Can't detect, assume door is closing
    _currentState = Closing;
    _updateAfterDoorMove();
  }
}

void Door::check() {
  Door::State newState{_getSensor()};

  // Ignore open sensor while door opening/closing
  if (newState == Open && _moving()) return;

  if (_currentState == Closed && newState == Open) {
    _currentState = Opening;
    _updateAfterDoorMove();
  } else {
    _currentState = newState;
  }
}

void Door::processChange() {
  if (_currentState == _lastState || _changeCallback == nullptr) return;
  _changeCallback(get());
}

Door::State Door::_getSensor() {
  // HIGH = sensor triggered = door closed
  return (Door::State) invertedRead(SENSOR);
}

bool Door::_moving() {
  return _currentState > Open;
}

void Door::_updateAfterDoorMove() {
  // Give door time to open/close before rechecking state
  _movementTimer.once(DOOR_MOVE_TIME, [this]() {
    _currentState = _getSensor();
  });
}
