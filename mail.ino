#include "creds.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "Wifi.h"






#define AWS_IOT_PUBLISH_TOPIC "esp32_pub"

#define AWS_IOT_SUBSCRIBE_TOPIC "esp32_sub"

#define DOOR_SENSOR_PIN 19
#define DEBOUNCE_DELAY 50

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Skapa en message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }


  Serial.println("AWS IoT Connected!");
}



void publishMessageOpen() {
  StaticJsonDocument<200> doc;
  doc["New mail"] = "You've got new mail!";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);  // skriv till klient

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println(jsonBuffer);
}


void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];

  Serial.println(message);

}


void setup() {
  Serial.begin(115200);
  connectAWS();
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
}

void loop() {
  static int lastDoorState = LOW;  // Spara senaste lästa tillståndet
  static unsigned long lastDebounceTime = 0;  // Tidpunkt för senaste förändringen

  int currentDoorState = digitalRead(DOOR_SENSOR_PIN);

  // Om läst tillstånd skiljer sig från det senaste sparade tillståndet, återställ debounce timer
  if (currentDoorState != lastDoorState) {
    lastDebounceTime = millis();
  }

  // Kontrollera om tillräckligt med tid har passerat sedan senaste förändringen
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // Om det faktiska tillståndet har varit stabilt tillräckligt länge, behandla det som en giltig förändring
    if (currentDoorState == HIGH) {
      Serial.println("Mailbox opened, new mail");
      publishMessageOpen();
    }
  }

  lastDoorState = currentDoorState;  // Spara det nuvarande tillståndet som det senaste

  client.loop();
  delay(1000); 
}