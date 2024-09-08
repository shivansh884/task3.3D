#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* wifiSSID = "realme";             // Your WiFi network SSID
const char* wifiPassword = "Realme25";       // Your WiFi network password

// MQTT broker details
const char* mqttBroker = "broker.emqx.io";  // MQTT broker address
const int mqttPortNumber = 1883;             // MQTT broker port number
const char* mqttTopicWaveAlert = "SIT210/waves2310994814";  // MQTT topic for Wave alerts
const char* mqttTopicPatAlert = "SIT210/pats2310994814";    // MQTT topic for  Pat alerts

WiFiClient wifiClient;                   
PubSubClient mqttClient(wifiClient);       // Create an MQTT client using the WiFi client

// Pin definition for LED
const int ledIndicatorPin = 2;              

void setup() 
{
  
  Serial.begin(115200);

  // Initialize LED pin as an output
  pinMode(ledIndicatorPin, OUTPUT);

  // Connect to WiFi network
  connectToWiFi();

  // Setup MQTT client with the broker details 
  mqttClient.setServer(mqttBroker, mqttPortNumber);
  mqttClient.setCallback(mqttMessageHandler);

  // Connect to MQTT broker
  connectToMQTT();
}

void loop() 
{
  // Reconnect to MQTT broker if the connection is lost
  if (!mqttClient.connected()) 
  {
    connectToMQTT();
  }
  
  // Handle incoming messages and maintain the MQTT connection
  mqttClient.loop();
}

void connectToWiFi()
{
 delay(10);  
 Serial.println("Connecting to WiFi...");
 WiFi.begin(wifiSSID, wifiPassword);  
do 
{
    delay(1000);  // Delay of 1 second 
    Serial.println("Connecting...");
} 
  while (WiFi.status() != WL_CONNECTED);
  Serial.println("Connected to WiFi");     // Print the connection status once connected
}

// Function to connect to the MQTT broker
void connectToMQTT() 
{
  // Try to connect to the MQTT broker until successful
  while (!mqttClient.connected())
  {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ReceiverClient"))  // Attempt to connect with a client ID
    {
      Serial.println("Connected to MQTT");

      // Subscribe to the MQTT topics
      mqttClient.subscribe(mqttTopicWaveAlert);  // Subscribe to Wave alert topic
      mqttClient.subscribe(mqttTopicPatAlert);   // Subscribe to Pat alert topic
    } 
    else 
    {
      delay(5000);  // delay of 5 seconds 
    }
  }
}

// Callback function to handle incoming MQTT messages
void mqttMessageHandler(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message received on topic ");
  Serial.print(topic);
  Serial.print(": ");
  
  // Print the message 
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Determine action based on the topic
  if (strcmp(topic, mqttTopicWaveAlert) == 0)  
  {
    Serial.println("Wave alert detected. LED flashing 3 times.");
    flashLED(3);  // Flash the LED 3 times for a "Wave" alert
  } 
  else if (strcmp(topic, mqttTopicPatAlert) == 0) 
  {
    Serial.println("Pat alert detected. LED flashing 5 times.");
    flashLED(5);  // Flash the LED 5 times for a "Pat" alert
  }
}

// Function to flash the LED
void flashLED(int flashCount) 
{
  for (int i = 0; i < flashCount; i++)
  {
    digitalWrite(ledIndicatorPin, HIGH);  
    delay(500);                           
    digitalWrite(ledIndicatorPin, LOW);   
    delay(500);                           
  }
}
