#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "config.h"
#include "MqttClient.h"
#include "Door.h"
#include "Watchdog.h"

MQTTClient mqtt;
Door door;
Watchdog watchdog;

Ticker stateUpdate;
Ticker sysUpdate;
bool updateInProgress{false};
char uid[16];

void setup() {
  sprintf(uid, "shelly_%06X", ESP.getChipId());

  LOG_BEGIN(115200);
  LOG_F("\n\nUnit ID: %s\n", uid);

  // Initialize door sensor and relay
  door.begin();

  LOG("Connecting to wifi: " WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(uid);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  watchdog.start();
  while (WiFi.waitForConnectResult(1000) != WL_CONNECTED) LOG(".");

  watchdog.stop();
  LOG("done\nIP Address is "); LOG_LN(WiFi.localIP());

  // Setup OTA updates
  ArduinoOTA.setHostname(uid);

  ArduinoOTA.onStart([]() {
    LOG_LN("OTA update initiated...");

    mqtt.disconnect();
    updateInProgress = true;
  });

  ArduinoOTA.onEnd([]() {
    LOG_LN("\nOTA update done, restarting...");
  });

  ArduinoOTA.onProgress([](auto progress, auto total) {
    LOG_F("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](auto error) {
    LOG_F("OTA error: [%u] ", error);
    switch(error) {
      case OTA_AUTH_ERROR:
        LOG_LN("Auth Failed"); break;
      case OTA_BEGIN_ERROR:
        LOG_LN("Begin Failed"); break;
      case OTA_CONNECT_ERROR:
        LOG_LN("Connect Failed"); break;
      case OTA_RECEIVE_ERROR:
        LOG_LN("Receive Failed"); break;
      case OTA_END_ERROR:
        LOG_LN("End Failed"); break;
    }

    ESP.restart();
  });

  ArduinoOTA.begin(false);

  // Set callback to run when mqtt command received
  mqtt.setCommandCallback([](auto state) {
    door.set(state);
  });

  // Set callback to run when circuit state changes
  door.setChangeCallback([](auto state) {
    mqtt.sendState(state);
  });

  // Check door sensor every quarter second
  stateUpdate.attach_ms(250, []() {
    door.check();
  });

  // Update system attributes every 10 seconds
  sysUpdate.attach(10, []() {
    mqtt.sendSys();
  });
}

void loop() {
  if (!WiFi.isConnected()) {
    watchdog.start();
    if (WiFi.waitForConnectResult() != WL_CONNECTED) return;
  }

  watchdog.stop();

  ArduinoOTA.handle();
  if (updateInProgress) return; // Disable mqtt handling during update

  // (Re)connect to mqtt
  if (!mqtt.connected()) {
    if (!mqtt.connect(uid)) {
      delay(100);
      return;
    }

    // Send all info on (re)connection
    mqtt.sendSys();
    mqtt.sendDiscovery(uid);
    mqtt.sendState(door.get());
  }

  mqtt.loop();
  door.processChange();
}
