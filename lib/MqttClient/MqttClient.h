#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFiClient.h>
#include <PubSubClient.h>
#include "config.h"
#include "Door.h"

class MQTTClient {
  public:
    MQTTClient();
    /*!
      \param cid Client ID
    */
    bool connect(String cid);
    void disconnect();
    bool connected();
    void loop();
    // Send door state
    void sendState(size_t state);
    // Send system info
    void sendSys();
    // Send Home Assistant discovery info
    void sendDiscovery(String uid);
    void setCommandCallback(Door::callback_t cb);

  private:
    static const String _baseTopic;
    static const String _commandTopic;
    static const String _stateTopic;
    static const String _sysTopic;
    static const String _upTopic;
    static const String _states[4];

    WiFiClient _wifiClient;
    PubSubClient _pubSubClient;
    Door::callback_t _cmdCallback;
};

#endif
