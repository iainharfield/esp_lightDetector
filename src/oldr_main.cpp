
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

//***********************
// Application functions
//**********************
bool onMqttMessageAppExt(char *, char *, const AsyncMqttClientMessageProperties &, const size_t &, const size_t &, const size_t &);    // Required by template
void appMQTTTopicSubscribe();
int sensorRead();
void telnet_extension_1(char);      // Required by template
void telnet_extension_2(char);      // Required by template
void telnet_extensionHelp(char);    // Required by template
void processTOD_Ext();              // Required by template

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
const char *oh3StateValue = "/house/ldr/daylight-front/value";  //FIXTHIS
String deviceName = "daylight-front";
String deviceType = "LDR";

Ticker sensorReadTimer;
int sensorValue;

devConfig espDevice;

void setup()
{
    bool configWiFi = false;
    Serial.begin(115200);
    while (!Serial)
        delay(300);

    espDevice.setup(deviceName, deviceType);

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
bool onMqttMessageAppExt(char *topic, char *payload, const AsyncMqttClientMessageProperties &properties, const size_t &len, const size_t &index, const size_t &total)
{
    return false;
}

// Subscribe to application specific topics
void appMQTTTopicSubscribe()
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
    printTelnet((String) "x\t\tSome description");
}

void drdDetected()
{
    Serial.println("Double resert detected");
}


//**********************************************************************
// Main Application
// chect current time against configutation and decide what to do.
// Send appropriate MQTT command for each control.
// Run this everytime the TOD changes
// If TOD event is not received then nothing happens
//**********************************************************************
void processTOD_Ext()
{
    // Nothing to do for this app
}

 
