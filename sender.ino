#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

// WiFi and MQTT credentials
char wifiNetwork[] = "realme";
char wifiPass[] = "Realme25";

// Pins for the ultrasonic sensor and LED
const int trigPin = 6;
const int echoPin = 8;
const int ledPin = 2;

// Variables for distance calculation
float pulseDuration, measuredDistance;
float soundSpeed = 0.0343;

// WiFi and MQTT clients
WiFiClient wifiConnection;
MqttClient mqttClient(wifiConnection);

String previousMessage = "";    // To store the last message sent
const char mqttServer[] = "broker.emqx.io";   // MQTT broker and topics

int mqttServerPort = 1883;
const char mqttTopicWave[] = "SIT210/waves2310994814";    // MQTT topic for sending Wave messages
const char mqttTopicPat[] = "SIT210/pats2310994814";      // MQTT topic for sending Pat messages

// Timing variables for interval control
const long measurementInterval = 1000;
long lastMeasurementTime = 0;

void setup() 
{
  Serial.begin(9600);

  // Set the trigger and echo pins for the ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to WiFi and the MQTT broker
  connectToWiFi();
  connectToMQTTServer();
}

void loop() 
{
  // Ensure the MQTT client is connected
  if (!mqttClient.connected())
  {
    connectToMQTTServer();
  }
  mqttClient.poll();

  // Get the current time
  long currentTime = millis();

  // Take a measurement if enough time has passed
  if (currentTime - lastMeasurementTime >= measurementInterval) 
  {
    lastMeasurementTime = currentTime;

    // Measure the distance
    measuredDistance = getDistance();

    Serial.print("Measured Distance: ");
    Serial.println(measuredDistance);

    // Send a message based on the distance
    sendMessageByDistance();
  }
}

// Connect to the WiFi network
void connectToWiFi() 
{
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifiNetwork);

  // Try to connect to WiFi
  do 
  {
    Serial.print(".");
    delay(1000);
  } 
  while (WiFi.begin(wifiNetwork, wifiPass) != WL_CONNECTED);

  Serial.println("WiFi Connected");
}

// Connect to the MQTT broker
void connectToMQTTServer() 
{
  Serial.print("Connecting to MQTT Broker: ");
  Serial.println(mqttServer);

  // Try to connect to the MQTT server
  do 
  {
    Serial.print(".");
    delay(1000);
  } 
  while (!mqttClient.connect(mqttServer, mqttServerPort));

  Serial.println("MQTT Broker Connected");
}

// Measure the distance using the ultrasonic sensor
float getDistance() 
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(6);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pulseDuration = pulseIn(echoPin, HIGH, 25000);

  // Calculate the distance based on the time taken 
  return (pulseDuration * soundSpeed) / 2;
}

// Send a message based on the measured distance
void sendMessageByDistance() 
{
  if (measuredDistance > 0 && measuredDistance < 17) 
  {
    // Send "Wave" message if in range and it hasn't been sent already
    if (previousMessage != "Wave") 
    {
      mqttClient.beginMessage(mqttTopicWave);
      mqttClient.print("shivansh_singh"); 
      mqttClient.endMessage();
      previousMessage = "Wave";
      Serial.println("Wave Signal Transmitted");
      delay(4000); 
    }
  } 
  else if (measuredDistance >= 17 && measuredDistance < 40) 
  {
    // Send Pat message if the distance is within the specified range

    if (previousMessage != "Pat") 
    {
      mqttClient.beginMessage(mqttTopicPat);
      mqttClient.print("shivansh_pat"); 
      mqttClient.endMessage();
      previousMessage = "Pat";
      Serial.println("Pat Message Sent");
      delay(4000); 
    }
  }
}
