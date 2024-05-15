#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10); // Пины подключения модуля nRF24L01
#define RELAY_PIN1 2



byte address[] = "1Node"; // Адрес передатчика

bool receivedValue; // Переменная для хранения принятого булевского значения

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(32);
  radio.openReadingPipe(1, address);
  radio.setChannel(0x60);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.powerUp();
  radio.startListening();
  // РЕЛЕ
  pinMode(RELAY_PIN1, OUTPUT);
  digitalWrite(RELAY_PIN1, 1);
}

void loop() {
  if (radio.available()) {
    radio.read(&receivedValue, sizeof(receivedValue));
    Serial.print("Received: ");
    Serial.println(receivedValue);
  }
  if (receivedValue){
    digitalWrite(RELAY_PIN1, 0);
  }
  else if (receivedValue == false){
    digitalWrite(RELAY_PIN1, 1);
  }
}
