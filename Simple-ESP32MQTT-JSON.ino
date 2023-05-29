#include <MQTT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>

// Configuration for connect to MQTT server
#define BROKER_IP    "YourServerURL"
#define BROKER_PORT  1883
#define DEV_NAME     "YoutDeviceName"
#define MQTT_USER    "YouruserName"
#define MQTT_PW      "YourPassword"

// Set your WiFi here
const char ssid[] = "YourSSID";
const char pass[] = "YourPassword";

String subTopic[] = {"/hello", "button"}; // Insert your topic name for Subscription

WiFiClient net;
MQTTClient client;

// Connect to MQTT server
void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nconnecting...");
  while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");

  for (int i = 0; i < sizeof subTopic / sizeof subTopic[0]; i++) {
    String topic = subTopic[i];
    client.subscribe(topic);
  }
}

// Message handler
void messageReceived(String &topic, String &payload) {
  //Serial.println("incoming: " + topic + " - " + payload);
  JSONVar myObject = JSON.parse(payload);
  //Serial.println(JSON.typeof(myObject));
  Serial.println(myObject);

  // Condition if message received from "button" topic, it will turn on / off built in led
  if (topic == "button") {
    if (String((const char*)myObject["name"]) == "esp32" && String((const char*)myObject["hw"]) == "builtInLED") {
      ((int) myObject["state"] == 1) ? digitalWrite(LED_BUILTIN, HIGH) : digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  pinMode(LED_BUILTIN, OUTPUT);
  client.begin(BROKER_IP, BROKER_PORT, net);
  client.onMessage(messageReceived);
  connect();
  client.publish("status", "Connected!");
}

void loop() {
  client.loop();
  if (!client.connected()) {
    connect();
  }
}
