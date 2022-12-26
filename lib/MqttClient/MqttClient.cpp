#include "MqttClient.h"
#include <ESP8266WiFi.h>

const String MQTTClient::_baseTopic{MQTT_BASE_TOPIC};
const String MQTTClient::_commandTopic{MQTT_BASE_TOPIC MQTT_CMD_SUF};
const String MQTTClient::_stateTopic{MQTT_BASE_TOPIC MQTT_STATE_SUF};
const String MQTTClient::_sysTopic{MQTT_BASE_TOPIC MQTT_ATTR_SUF};
const String MQTTClient::_upTopic{MQTT_BASE_TOPIC MQTT_AVTY_SUF};
const String MQTTClient::_states[]{MQTT_MSG_CLOSED, MQTT_MSG_OPEN, MQTT_MSG_CLOSING, MQTT_MSG_OPENING};

MQTTClient::MQTTClient() :
  _wifiClient{},
  _pubSubClient{_wifiClient, MQTT_SERVER, MQTT_PORT},
  _cmdCallback{nullptr}
{
  _pubSubClient.set_callback([this](const MQTT::Publish& pub) {
    String cmd{pub.payload_string()};
    if (cmd == MQTT_MSG_RESTART) ESP.restart();
    if (_cmdCallback == nullptr || (cmd != MQTT_CMD_OPEN && cmd != MQTT_CMD_CLOSE)) return;

    _cmdCallback((Door::State)(cmd == MQTT_CMD_OPEN)); // Cast bool to first two door states
  });
}

bool MQTTClient::connect(String cid) {
  if (_pubSubClient.connected()) return true;

  LOG_F("Connecting to MQTT broker %s... ", MQTT_SERVER);
  MQTT::Connect conn{cid};
  conn.set_auth(MQTT_USER, MQTT_PASS)
      .set_will(_upTopic, MQTT_MSG_DOWN, MQTT_QOS, MQTT_RETAIN);

  if (!_pubSubClient.connect(conn)) {
    LOG_LN("FAILED!");
    return false;
  }

  LOG_F("Subscribing to topic %s\n", _commandTopic.c_str());
  _pubSubClient.subscribe(_commandTopic, MQTT_QOS);
  _pubSubClient.publish(
    MQTT::Publish{_upTopic, MQTT_MSG_UP}
      .set_retain(MQTT_RETAIN)
      .set_qos(MQTT_QOS)
  );
  return true;
}

void MQTTClient::disconnect() {
  _pubSubClient.publish(
    MQTT::Publish{_upTopic, MQTT_MSG_DOWN}
      .set_retain(MQTT_RETAIN)
      .set_qos(MQTT_QOS)
  );
  _pubSubClient.disconnect();
}

bool MQTTClient::connected() {
  return _pubSubClient.connected();
}

void MQTTClient::loop() {
  _pubSubClient.loop();
}

void MQTTClient::sendState(size_t state) {
  _pubSubClient.publish(
    MQTT::Publish{_stateTopic, _states[state]}
      .set_retain(MQTT_RETAIN)
      .set_qos(MQTT_QOS)
  );
  LOG_F("Door %s\n", _states[state].c_str());
}

void MQTTClient::sendSys() {
  String payload{"{"
    "\"IP\":\"" + WiFi.localIP().toString() + "\","
    "\"RSSI\":" + String{WiFi.RSSI()} + ","
    "\"FreeMem\":" + String{ESP.getFreeHeap()} +
  "}"};
  _pubSubClient.publish(_sysTopic, payload);
}

void MQTTClient::sendDiscovery(String uid) {
  String topic{"homeassistant/cover/" + uid + "/config"};
  String payload{"{"
    "\"name\":\"" NAME "\","
    "\"uniq_id\":\"" + uid + "\","
    "\"dev_cla\":\"" DEVICE_CLASS "\","
    "\"~\":\"" + _baseTopic + "\","
    "\"cmd_t\":\"~" MQTT_CMD_SUF "\","
    "\"stat_t\":\"~" MQTT_STATE_SUF "\","
    "\"json_attr_t\":\"~" MQTT_ATTR_SUF "\","
    "\"avty_t\":\"~" MQTT_AVTY_SUF "\","
    #ifndef MQTT_MSG_OPEN_DEFAULT
    "\"stat_open\":\"" MQTT_MSG_OPEN "\","
    #endif
    #ifndef MQTT_MSG_OPENING_DEFAULT
    "\"stat_opening\":\"" MQTT_MSG_OPENING "\","
    #endif
    #ifndef MQTT_MSG_CLOSED_DEFAULT
    "\"stat_clsd\":\"" MQTT_MSG_CLOSED "\","
    #endif
    #ifndef MQTT_MSG_CLOSING_DEFAULT
    "\"stat_closing\":\"" MQTT_MSG_CLOSING "\","
    #endif
    #ifndef MQTT_CMD_OPEN_DEFAULT
    "\"pl_open\":\"" MQTT_CMD_OPEN "\","
    #endif
    #ifndef MQTT_CMD_CLOSE_DEFAULT
    "\"pl_close\":\"" MQTT_CMD_CLOSE "\","
    #endif
    "\"pl_stop\":null,"
    #ifndef MQTT_MSG_UP_DEFAULT
    "\"pl_avail\":\"" MQTT_MSG_UP "\","
    #endif
    #ifndef MQTT_MSG_DOWN_DEFAULT
    "\"pl_not_avail\":\"" MQTT_MSG_DOWN "\","
    #endif
    "\"qos\":" STR(MQTT_QOS) ","
    "\"dev\":{"
      "\"name\":\"" DEVICE_NAME "\","
      "\"mf\":\"Shelly\","
      "\"mdl\":\"" DEVICE_MODEL "\","
      "\"ids\":[\"" + ESP.getChipId() + "\"],"
      "\"cns\":[[\"mac\",\"" + WiFi.macAddress() + "\"]],"
      "\"sw\":\"" VERSION "\""
    "}"
  "}"};

  _pubSubClient.publish(
    MQTT::Publish{topic, payload}.set_retain()
  );
}

void MQTTClient::setCommandCallback(Door::callback_t cb) {
  _cmdCallback = cb;
}
