
//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <time.h>
#include <AsyncMqttClient_Generic.hpp>

#define ESP8266_DRD_USE_RTC true
#define ESP_DRD_USE_LITTLEFS false
#define ESP_DRD_USE_SPIFFS false
#define ESP_DRD_USE_EEPROM false
#define DOUBLERESETDETECTOR_DEBUG true
#include <ESP_DoubleResetDetector.h>

#include "defines.h"
#include "utilities.h"

bool onMqttMessageExt(char *, char *, const AsyncMqttClientMessageProperties &, const size_t &, const size_t &, const size_t &);
int sensorRead();
void telnet_extension_1(char);
void telnet_extension_2(char);
void telnet_extensionHelp(char);

// defined in asyncConnect.cpp
extern void mqttTopicsubscribe(const char *topic, int qos);
extern void platform_setup(bool);
extern void handleTelnet();
extern void printTelnet(String);
extern AsyncMqttClient mqttClient;
extern void wifiSetupConfig(bool);

#define DRD_TIMEOUT 3
#define DRD_ADDRESS 0

DoubleResetDetector *drd;

// defined in telnet.cpp
extern int reporting;
extern bool telnetReporting;

// Application Specific MQTT Topics and config
const char *oh3StateValue = "/house/ldr/daylight-front/value";
const char *sensorName = "daylight-front";
const char *sensorType = "LDR";

Ticker sensorReadTimer;
int sensorValue;


devConfig sensor;

void setup()
{
    bool configWiFi = false;
    Serial.begin(115200);
    while (!Serial)
        delay(300);

    sensor.setup(sensorName, sensorType);

    Serial.println("\nStarting Daylight detector on ");
    Serial.println(ARDUINO_BOARD);

    drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
    if (drd->detectDoubleReset())
    {
        configWiFi = true;
    }

    // Platform setup: Set up and manage: WiFi, MQTT and Telnet
    platform_setup(configWiFi);

    // Application setup
    pinMode(A0, INPUT);
    sensorReadTimer.attach(90, sensorRead); // read every 5 mins
}

void loop()
{
    drd->loop();

    // Go look for OTA request
    ArduinoOTA.handle();

    handleTelnet();
}

// Process any application specific inbound MQTT messages
// Return False if none
// Return true if an MQTT message was handled here
bool onMqttMessageExt(char *topic, char *payload, const AsyncMqttClientMessageProperties &properties, const size_t &len, const size_t &index, const size_t &total)
{
    return false;
}

// Subscribe to application specific topics
void mqttTopicSubscribe()
{

    // mqttTopicsubscribe(oh3StateValue, 2);
}

int sensorRead()
{
    char logString[MAX_LOGSTRING_LENGTH];

    sensorValue = analogRead(A0); // read analog input pin 0

    if (reporting == REPORT_DEBUG)
    {
        memset(logString, 0, sizeof logString);
        sprintf(logString, "%s,%d", "LDR Detected", sensorValue);
        mqttLog(logString, true, true);
    }

    // Publish value for reporting
    sprintf(logString, "%d", sensorValue);

    mqttClient.publish(oh3StateValue, 1, true, logString);

    return sensorValue;
}

// Write out ove telnet session and application specific infomation
void telnet_extension_1(char c)
{
    char logString[MAX_LOGSTRING_LENGTH];
    memset(logString, 0, sizeof logString);
    sprintf(logString, "%s%d\n\r", "Sensor Value:\t", sensorValue);
    printTelnet((String)logString);
}

// Process any application specific telnet commannds
void telnet_extension_2(char c)
{
    printTelnet((String)c);
}

// Process any application specific telnet commannds
void telnet_extensionHelp(char c)
{
    printTelnet((String) "x\t\tSome description0");
}

void drdDetected()
{
    Serial.println("Double resert detected");
}
