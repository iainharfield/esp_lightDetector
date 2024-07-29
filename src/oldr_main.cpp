
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <AsyncMqttClient.h> 
#include <time.h>

#include "hh_defines.h"
#include "hh_utilities.h"
#include "hh_cntrl.h"

// Folling line added to stop compilation error suddenly occuring in 2024???
#include "ESPAsyncDNSServer.h"

#define ESP8266_DRD_USE_RTC true
#define ESP_DRD_USE_LITTLEFS false
#define ESP_DRD_USE_SPIFFS false
#define ESP_DRD_USE_EEPROM false
#define DOUBLERESETDETECTOR_DEBUG true
#include <ESP_DoubleResetDetector.h>

//***********************
// Template functions
//***********************
bool onMqttMessageAppExt(char *, char *, const AsyncMqttClientMessageProperties &, const size_t &, const size_t &, const size_t &);
bool onMqttMessageAppCntrlExt(char *, char *, const AsyncMqttClientMessageProperties &, const size_t &, const size_t &, const size_t &);
void appMQTTTopicSubscribe();
void telnet_extension_1(char);
void telnet_extension_2(char);
void telnet_extensionHelp(char);
void startTimesReceivedChecker();
void processCntrlTOD_Ext();

//***********************
// Application functions
//**********************
int sensorRead();
//void processTOD_Ext();              // Required by template

//******************************
// defined in asyncConnect.cpp
//******************************
extern void mqttTopicsubscribe(const char *topic, int qos);
extern void platform_setup(bool);
extern void handleTelnet();
extern void printTelnet(String);
extern AsyncMqttClient mqttClient;
extern void wifiSetupConfig(bool);
extern templateServices coreServices;
extern char ntptod[MAX_CFGSTR_LENGTH];


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

    sprintf(logString, "%s,%d", "LDR Detected", sensorValue);
    mqttLog(logString, REPORT_DEBUG, true, true);

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

void processCntrlTOD_Ext()
{
    // Nothing to do for this app
}

bool onMqttMessageAppCntrlExt(char *topic, char *payload, const AsyncMqttClientMessageProperties &properties, const size_t &len, const size_t &index, const size_t &total)
{
	// Nothing to do for this app
    return false;
}
 
