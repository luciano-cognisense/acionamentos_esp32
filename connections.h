
#ifndef connections_h
#define connections_h

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define ESP_DRD_USE_SPIFFS true
#include <FS.h> // File System Library
#include <SPIFFS.h> // SPI Flash Syetem Library
#include <WiFiManager.h> // WiFiManager Library
#include <ArduinoJson.h> // Arduino JSON library

extern WiFiManager wm;
extern WiFiClient wifiClient;
extern PubSubClient mqttClient;
extern char *mqttServer;
extern int mqttPort;
extern const char* mqttUser;
extern const char* mqttPassword;
#define RELAY_1 27
#define RELAY_2 25
#define RELAY_3 32
#define RELAY_4 12
#define WIFI_LED 26
#define LED_1 5//18
#define LED_2 23//19
#define LED_3 19//23
#define LED_4 18//5

// =================== Prototyped Functions ====================
void setupMQTT();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void saveConfigFile();
bool loadConfigFile();
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);
void setupWifiManager();
void setModulePins();
void setRelay(int pin, bool state);
void processData(String message);


// =============================================================

#endif
