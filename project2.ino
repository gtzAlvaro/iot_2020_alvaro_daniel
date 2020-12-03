#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

long last_time = 0;
const int plant_1_gpio = 36;
const int plant_2_gpio = 39;
const int plant_3_gpio = 34;
const int plant_4_gpio = 35;

const int relay_1_gpio = 4;
const int relay_2_gpio = 0;
const int relay_3_gpio = 2;
const int relay_4_gpio = 15;

int plant_1 = 0;
int plant_2 = 0;
int plant_3 = 0;
int plant_4 = 0;

char value[10];

const char *SSID = "inalambrica";
const char *PWD = "silvia25";
void connectToWiFi() {
  Serial.print("Connecting to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
  
}

// MQTT client
WiFiClient wifiClient;
WiFiClient wifiClient2;
PubSubClient mqttClient(wifiClient); 
PubSubClient mqttClient2(wifiClient2);
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);

  mqttClient2.setServer("mqtt.thingspeak.com", mqttPort);
  // set the callback function
  mqttClient2.setCallback(callback2);
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  setupMQTT();

  pinMode(relay_1_gpio, OUTPUT);
  pinMode(relay_2_gpio, OUTPUT);
  pinMode(relay_3_gpio, OUTPUT);
  pinMode(relay_4_gpio, OUTPUT);

  digitalWrite(relay_1_gpio, HIGH);
  digitalWrite(relay_2_gpio, HIGH);
  digitalWrite(relay_3_gpio, HIGH);
  digitalWrite(relay_4_gpio, HIGH);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("ITESO/IoT/GreenLife/1");
        mqttClient.subscribe("ITESO/IoT/GreenLife/2");
        mqttClient.subscribe("ITESO/IoT/GreenLife/3");
        mqttClient.subscribe("ITESO/IoT/GreenLife/4");
      }
  }

  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient2.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient2.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
      }
  }
}

void callback2(char* topic, byte* payload, unsigned int length){
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if((char)topic[20] == '1'){
    digitalWrite(relay_1_gpio, LOW);
    delay(250);
    digitalWrite(relay_1_gpio, HIGH);
    Serial.println("Activated relay 1");
  }else if((char)topic[20] == '2'){
    digitalWrite(relay_2_gpio, LOW);
    delay(250);
    digitalWrite(relay_2_gpio, HIGH);
    Serial.println("Activated relay 2");
  }else if((char)topic[20] == '3'){
    digitalWrite(relay_3_gpio, LOW);
    delay(250);
    digitalWrite(relay_3_gpio, HIGH);
    Serial.println("Activated relay 3");
  }else if((char)topic[20] == '4'){
    digitalWrite(relay_4_gpio, LOW);
    delay(250);
    digitalWrite(relay_4_gpio, HIGH);
    Serial.println("Activated relay 4");
  }else{
    Serial.println("topic not supported");
  }
}

void loop() {
  if (!mqttClient.connected())
    reconnect();

  if (!mqttClient2.connected())
    reconnect();
  mqttClient.loop();
  mqttClient2.loop();

  long now = millis();
  if (now - last_time > 10000) {
    // Send data
    /*
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pres = bme.readPressure() / 100;
    */
    // Publishing data throgh MQTT
    plant_1 = analogRead(plant_1_gpio);
    plant_1 = map(plant_1, 0, 4095, 100, 0);
    sprintf(value, "%i", plant_1);
    Serial.println(value);
    mqttClient2.publish("channels/1233836/publish/fields/field1/ZVCF2X9LGUJP1EZ0", value);
    
    plant_2 = analogRead(plant_2_gpio);
    plant_2 = map(plant_2, 0, 4095, 100, 0);
    sprintf(value, "%i", plant_2);
    Serial.println(value);
    mqttClient2.publish("channels/1233836/publish/fields/field2/ZVCF2X9LGUJP1EZ0", value);
    
    plant_3 = analogRead(plant_3_gpio);
    plant_3 = map(plant_3, 0, 4095, 100, 0);
    sprintf(value, "%i", plant_3);
    Serial.println(value);
    mqttClient2.publish("channels/1233836/publish/fields/field3/ZVCF2X9LGUJP1EZ0", value);
    
    plant_4 = analogRead(plant_4_gpio);
    plant_4 = map(plant_4, 0, 4095, 100, 0);
    sprintf(value, "%i", plant_4);
    Serial.println(value);
    mqttClient2.publish("channels/1233836/publish/fields/field4/ZVCF2X9LGUJP1EZ0", value);
    last_time = now;
  }
}
