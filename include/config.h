#ifndef CONFIG_H
#define CONFIG_H

/*
  ====================================================================================================
                        Define constants below or, preferably, in platformio.ini
  ====================================================================================================
*/

//#define WIFI_SSID "wifi_ssid"           // Your WiFi ssid
//#define WIFI_PASS "wifi_password"       // Your WiFi password
//#define WIFI_TIMEOUT 10                 // Minutes to wait for a wifi connection before rebooting

//#define MQTT_SERVER "192.168.0.1"       // Your mqtt server ip address
//#define MQTT_PORT 1883                  // Your mqtt port
//#define MQTT_BASE_TOPIC "shop/garage"   // Base mqtt topic
//#define MQTT_USER "mqtt_user"           // mqtt username
//#define MQTT_PASS "password"            // mqtt password
//#define MQTT_RETAIN true                // Retain status/availability messages (recommended)
//#define MQTT_QOS 0                      // QOS level for all mqtt messages (0 or 1)

//#define NAME "Garage Door"              // Friendly name for device discovery
//#define DEVICE_CLASS "garage"           // Device class for discovery (subtype of MQTT cover)
//#define DOOR_MOVE_TIME 18               // Number of seconds it takes for the door to open/close
//#define PULSE_MS 300                    // Number of milliseconds to trigger the door opener

//#define DEBUG                           // Uncomment to enable debug reporting

/*
  ====================================================================================================
                                         END USER CONFIGURATION
  ====================================================================================================
*/

#include <Arduino.h>

/* ==== Wifi Config ==== */
#ifndef WIFI_SSID
  #define WIFI_SSID "wifi_ssid"
#endif
#ifndef WIFI_PASS
  #define WIFI_PASS "wifi_password"
#endif
#ifndef WIFI_TIMEOUT
  #define WIFI_TIMEOUT 10
#endif


/* ==== MQTT config ==== */
#ifndef MQTT_SERVER
  #define MQTT_SERVER "192.168.0.1"
#endif
#ifndef MQTT_PORT
  #define MQTT_PORT 1883
#endif
#ifndef MQTT_BASE_TOPIC
  #define MQTT_BASE_TOPIC "shop/garage"
#endif
#ifndef MQTT_USER
  #define MQTT_USER "mqtt_user"
#endif
#ifndef MQTT_PASS
  #define MQTT_PASS "password"
#endif
#ifndef MQTT_RETAIN
  #define MQTT_RETAIN true
#endif
#ifndef MQTT_QOS
  #define MQTT_QOS 0
#endif

// Home Assistant defaults
#ifndef MQTT_CMD_OPEN
  #define MQTT_CMD_OPEN "OPEN"
  #define MQTT_CMD_OPEN_DEFAULT
#endif
#ifndef MQTT_CMD_CLOSE
  #define MQTT_CMD_CLOSE "CLOSE"
  #define MQTT_CMD_CLOSE_DEFAULT
#endif
#ifndef MQTT_MSG_OPEN
  #define MQTT_MSG_OPEN "open"
  #define MQTT_MSG_OPEN_DEFAULT
#endif
#ifndef MQTT_MSG_OPENING
  #define MQTT_MSG_OPENING "opening"
  #define MQTT_MSG_OPENING_DEFAULT
#endif
#ifndef MQTT_MSG_CLOSED
  #define MQTT_MSG_CLOSED "closed"
  #define MQTT_MSG_CLOSED_DEFAULT
#endif
#ifndef MQTT_MSG_CLOSING
  #define MQTT_MSG_CLOSING "closing"
  #define MQTT_MSG_CLOSING_DEFAULT
#endif
#ifndef MQTT_MSG_UP
  #define MQTT_MSG_UP "online"
  #define MQTT_MSG_UP_DEFAULT
#endif
#ifndef MQTT_MSG_DOWN
  #define MQTT_MSG_DOWN "offline"
  #define MQTT_MSG_DOWN_DEFAULT
#endif

// Not used by Home Assistant
#ifndef MQTT_MSG_RESTART
  #define MQTT_MSG_RESTART "restart"
#endif

// Other MQTT constants
#define MQTT_CMD_SUF "/cmd"
#define MQTT_STATE_SUF "/stat"
#define MQTT_ATTR_SUF "/sys"
#define MQTT_AVTY_SUF "/up"


/* ==== Device config ==== */
#ifndef NAME
  #define NAME "Garage Door"
#endif
#ifndef DEVICE_CLASS
  #define DEVICE_CLASS "garage"
#endif
#ifndef DEVICE_MODEL
  #define DEVICE_MODEL "Shelly 1"
#endif
#ifndef DEVICE_NAME
  #define DEVICE_NAME DEVICE_MODEL
#endif
#ifndef DOOR_MOVE_TIME
  #define DOOR_MOVE_TIME 18
#endif
#ifndef PULSE_MS
  #define PULSE_MS 300
#endif


/* ==== Hardware constants ==== */
#define RELAY 4
#define SENSOR 5


/* ==== Other ==== */
#ifndef VERSION
  #define VERSION "20220412000000"
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifdef DEBUG
  #define LOG_BEGIN(x) Serial.begin(x)
  #define LOG(x) Serial.print(x)
  #define LOG_LN(x) Serial.println(x)
  #define LOG_F(x, ...) Serial.printf(x, __VA_ARGS__)
#else
  #define LOG_BEGIN(x)
  #define LOG(x)
  #define LOG_LN(x)
  #define LOG_F(x, ...)
#endif

#endif
