//slave
#include <WiFi.h>
#include <esp_now.h>
#include <NewPing.h>
 
#define TRIGGER_PIN 27
#define ECHO_PIN 26
#define MAX_DISTANCE 200

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0xA9, 0xCE, 0xAC};

unsigned int actual_distance_cm = 100;

typedef struct struct_message {
  unsigned int distance_cm;
} struct_message;

struct_message distance_message;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  actual_distance_cm = sonar.ping_cm();
  Serial.print("Actual distance: ");
  Serial.println(actual_distance_cm);
  distance_message.distance_cm = actual_distance_cm;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &distance_message, sizeof(distance_message));

  if (result != ESP_OK){
    Serial.println("Error during sending data");
  } else {
    Serial.println("Data sent");
  }

  delay(1000);
}
