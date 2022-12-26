#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Ticker.h>

class Watchdog {
  public:
    Watchdog();
    void start();
    void stop();

  private:
    Ticker _timer;
};

#endif
