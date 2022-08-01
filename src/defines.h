/****************************************************************************************************************************
  defines.h
  AsyncMqttClient_Generic is a library for ESP32, ESP8266, Protenta_H7, STM32F7, etc. with current AsyncTCP support

  Based on and modified from :

  1) async-mqtt-client (https://github.com/marvinroger/async-mqtt-client)

  Built by Khoi Hoang https://github.com/khoih-prog/AsyncMqttClient_Generic
 ***************************************************************************************************************************************/

#ifndef defines_h
#define defines_h

#include <Arduino.h>
#include <TZ.h>

#define _ASYNC_MQTT_LOGLEVEL_ 1

#define MAX_CFGSTR_LENGTH 51
#define MAX_LOGSTRING_LENGTH 501

#define TELNET_PORT 23

#define MY_TZ TZ_Europe_London
#define MY_NTP_SERVER "at.pool.ntp.org"


#define p_mqttBrokerIP_Label         "mqttBrokerIP"
#define p_mqttBrokerPort_Label       "mqttBrokerPort"


// MQTT Topic Names
#define oh3CommandIOT "/house/service/iot-command"              // e.g. IOT-IDENTITY, IOT-RESET, IOT-RESTART, IOT-SWITCH-CONFIG
#define oh3StateLog   "/house/service/log"                        // Log messages
#define oh3CommandTOD "/house/service/time-of-day"              // Time of day broadcast from OpenHab
//#define oh3CommandIdentity "/house/ldr/daylight-front/identity" // Respomse message to IOT-IDENTITY

#define REPORT_INFO   0
#define REPORT_WARN   1
#define REPORT_DEBUG  2


class devConfig
{
  String devName;
  String devType;
  bool configWifiOnboot;

public:
  devConfig() {}
  devConfig(String name, String type)
  {
    devName = name;
    devType = type;
    configWifiOnboot = false;
  }
  void setup(String name, String type)
  {
    devName = name;
    devType = type;
  }
  String getName()
  {
    return devName;
  }
  String getType()
  {
    return devType;
  }
  void setwifiConfigOnboot(bool bValue)
  {
    configWifiOnboot = bValue;
  }
  bool getwifiConfigOnboot(bool bValue)
  {
    return configWifiOnboot;
  }

};
#endif // defines_h