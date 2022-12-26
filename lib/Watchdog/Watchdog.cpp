#include "Watchdog.h"
#include "config.h"

Watchdog::Watchdog() :
  _timer{Ticker{}}
{}

void Watchdog::start() {
  if (_timer.active()) return;
  _timer.once(WIFI_TIMEOUT*60, ESP.restart);
}

void Watchdog::stop() {
  _timer.detach();
}
