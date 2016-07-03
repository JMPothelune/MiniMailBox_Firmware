#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include "config.h"


WiFiClient espClient;
PubSubClient client(espClient);
Servo flagServo; 

#define PIN_SERVO  D8


const char* mqtt_server_host = "io.adafruit.com";
const int   mqtt_server_port = 1883;

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server_host, mqtt_server_port);
  client.setCallback(callback);
}


long lastMsg = 0;
char msg[50];
int value = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  raiseUpFlag();
  // TODO Protocol on message received
}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("MiniMailBox", mqtt_server_username, mqtt_server_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(status_topic, "connected");
      // ... and resubscribe
      client.subscribe(event_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void raiseUpFlag() {
  flagServo.attach(PIN_SERVO);
  flagServo.write(150);
  delay(250);
  flagServo.write(255);
  delay(250);
  
  flagServo.detach();
}

