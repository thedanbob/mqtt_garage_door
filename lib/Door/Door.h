#ifndef DOOR_H
#define DOOR_H

#include <Ticker.h>
#include <functional>
#include "config.h"

class Door {
  public:
    enum State {Closed, Open, Closing, Opening};
    typedef std::function<void (State state)> callback_t;

    Door();
    void begin();
    void setChangeCallback(callback_t cb);
    State get();
    void set(State state);
    // Check if external sensor has changed
    void check();
    void processChange();

  private:
    State _currentState;
    State _lastState;
    Ticker _movementTimer;
    callback_t _changeCallback;

    State _getSensor();
    bool _moving();
    void _updateAfterDoorMove();
};

#endif
