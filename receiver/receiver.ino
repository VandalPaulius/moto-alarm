// moto-alarm Receiver

#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define PAYLOAD_SIZE 31

RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int LED1 = 3;
char secret[] = "77da4ba6-fdf2-11e7-8be5-0ed5f";

void setup(void) {
    Serial.begin(9600);
    radio.begin();
    radio.openReadingPipe(1,pipe);
    radio.setPayloadSize(PAYLOAD_SIZE);
    radio.startListening();
    
    pinMode(LED1, OUTPUT);
}

void loop(void) {
    if (radio.available(pipe)) {
        bool done = false;
        char message[PAYLOAD_SIZE];

        while (!done) {
            done = radio.read(message, PAYLOAD_SIZE);      
            Serial.println(message);
            delay(10);
        }
    } else {
        Serial.println("No radio available");
    }
}
