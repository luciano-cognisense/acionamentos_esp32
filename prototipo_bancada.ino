
#include "connections.h"

void setup() {
  Serial.begin(115200);
  setupWifiManager();
  setupMQTT();
  mqttClient.subscribe("/25/relay");
  setModulePins();
}

void loop() {
  if (!mqttClient.connected()){
    digitalWrite(WIFI_LED, LOW);
    reconnect();
  }
  mqttClient.loop();
  
}
