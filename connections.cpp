#include "connections.h"
#include "Arduino.h"

// ========== WiFi Manager objects and variables ==============
// JSON configuration file
#define JSON_CONFIG_FILE "/test_config.json"
// Flag for saving data
bool shouldSaveConfig = false;
// Variables to hold data from custom textboxes
char testString[50] = "test value";
int testNumber = 1234;
WiFiManager wm;
WiFiClient wifiClient;
//const char * ssid = "FIESC_IOT";
//const char * password = "C6qnM4ag81";
// ============================================================

// ======== MQTT communication objects and variables ==========
PubSubClient mqttClient(wifiClient);
char *mqttServer = "broker.emqx.io";
int mqttPort = 1883;
const char* mqttUser = "labremoto";
const char* mqttPassword = "labremoto2023";
// =============================================================

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-1";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        digitalWrite(WIFI_LED, HIGH);
        mqttClient.subscribe("/25/relay");
      }
      
  }
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  String str_payload = "";
   
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    str_payload += String((char)payload[i]);
  }
  //String str_payload = String((char*)payload);
  Serial.println(str_payload);
  processData(str_payload);
}

void setRelay(int pin, bool state){
  pinMode(pin, OUTPUT);
  digitalWrite(RELAY_1, LOW);
}


void setModulePins(){
  pinMode(RELAY_1,OUTPUT);
  pinMode(RELAY_2,OUTPUT);
  pinMode(RELAY_3,OUTPUT);
  pinMode(RELAY_4,OUTPUT);
  pinMode(WIFI_LED,OUTPUT);
  pinMode(LED_1,OUTPUT);
  pinMode(LED_2,OUTPUT);
  pinMode(LED_3,OUTPUT);
  pinMode(LED_4,OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);
  digitalWrite(WIFI_LED, LOW);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  digitalWrite(LED_4, LOW);
}

void processData(String message){  
  if(message == String("relay1_on")){
    digitalWrite(RELAY_1, LOW);
    digitalWrite(LED_1, HIGH);
    Serial.println("relay_1_on");
  }else if(message == String("relay2_on")){
    digitalWrite(RELAY_2, LOW);
    digitalWrite(LED_2, HIGH);
    Serial.println("relay_2_on");
  }else if(message == String("relay3_on")){
    digitalWrite(RELAY_3, LOW);
    digitalWrite(LED_3, HIGH);
    Serial.println("relay_3_on");
  }else if(message == String("relay4_on")){
    digitalWrite(RELAY_4, LOW);
    digitalWrite(LED_4, HIGH);
    Serial.println("relay_4_on");
  }else if(message == String("relay1_off")){
    digitalWrite(RELAY_1, HIGH);
    digitalWrite(LED_1, LOW);
    Serial.println("relay_1_off");
  }else if(message == String("relay2_off")){
    digitalWrite(RELAY_2, HIGH);
    digitalWrite(LED_2, LOW);
    Serial.println("relay_2_off");
  }else if(message == String("relay3_off")){
    digitalWrite(RELAY_3, HIGH);
    digitalWrite(LED_3, LOW);
    Serial.println("relay_3_off");
  }else if(message == String("relay4_off")){
    digitalWrite(RELAY_4, HIGH);
    digitalWrite(LED_4, LOW);
    Serial.println("relay_4_off");
  }
}

void saveConfigFile(){
  Serial.println(F("Saving configuration..."));
  
  // Create a JSON document
  StaticJsonDocument<512> json;
  json["testString"] = testString;
  json["testNumber"] = testNumber;

  // Open config file
  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile)
  {
    // Error, file did not open
    Serial.println("failed to open config file for writing");
  }

  // Serialize JSON data to write to file
  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0)
  {
    // Error writing file
    Serial.println(F("Failed to write to file"));
  }
  // Close file
  configFile.close();
}

bool loadConfigFile(){
  // Uncomment if we need to format filesystem
  //SPIFFS.format();

  // Read configuration from FS json
  Serial.println("Mounting File System...");

  // May need to make it begin(true) first time you are using SPIFFS
  if (SPIFFS.begin(false) || SPIFFS.begin(true))
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists(JSON_CONFIG_FILE))
    {
      // The file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
      if (configFile)
      {
        Serial.println("Opened configuration file");
        StaticJsonDocument<512> json;
        DeserializationError error = deserializeJson(json, configFile);
        serializeJsonPretty(json, Serial);
        if (!error)
        {
          Serial.println("Parsing JSON");

          strcpy(testString, json["testString"]);
          testNumber = json["testNumber"].as<int>();

          return true;
        }
        else
        {
          // Error loading JSON data
          Serial.println("Failed to load json config");
        }
      }
    }
  }
  else
  {
    // Error mounting file system
    Serial.println("Failed to mount FS");
  }

  return false;
}

void saveConfigCallback(){
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager){
  Serial.println("Entered Configuration Mode");

  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());

  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void setupWifiManager(){
  // Change to true when testing to force configuration every time we run
  bool forceConfig = false;

  bool spiffsSetup = loadConfigFile();
  if (!spiffsSetup)
  {
    Serial.println(F("Forcing config mode as there is no saved config"));
    forceConfig = true;
  }

  // Explicitly set WiFi mode
  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid,password);
  Serial.println("1");

  // Reset settings (only for development)
  wm.resetSettings();
  Serial.println("2");

  // Set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);
  Serial.println("3");

  // Set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);
  Serial.println("4");

  // Custom elements
  // Text box (String) - 50 characters maximum
  WiFiManagerParameter custom_text_box("key_text", "Enter your string here", testString, 50);
  Serial.println("5");
  // Need to convert numerical input to string to display the default value.
  char convertedValue[6];
  sprintf(convertedValue, "%d", testNumber);
  Serial.println("6"); 
  
  // Text box (Number) - 7 characters maximum
  WiFiManagerParameter custom_text_box_num("key_num", "Enter your number here", convertedValue, 7); 
  Serial.println("7");
  
  // Add all defined parameters
  wm.addParameter(&custom_text_box);
  wm.addParameter(&custom_text_box_num);
  Serial.println("Antes Forçar configuração");
  if (forceConfig)
    // Run if we need a configuration
  {
    Serial.println("Forçar configuração");
    if (!wm.startConfigPortal("NEWTEST_AP", "password"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
  else
  {
    Serial.println("Else Forçar configuração");
    if (!wm.autoConnect("NEWTEST_AP", "password"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }

  // If we get here, we are connected to the WiFi

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Lets deal with the user config values

  // Copy the string value
  strncpy(testString, custom_text_box.getValue(), sizeof(testString));
  Serial.print("testString: ");
  Serial.println(testString);

  //Convert the number value
  testNumber = atoi(custom_text_box_num.getValue());
  Serial.print("testNumber: ");
  Serial.println(testNumber);


  // Save the custom parameters to FS
  if (shouldSaveConfig)
  {
    saveConfigFile();
  }
}
