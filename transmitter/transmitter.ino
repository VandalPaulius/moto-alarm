// moto-alarm Reciever

#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Push buttons
#define LOCK        2
#define UNLOCK      3
#define SENSITIVITY 4
#define SOUND       5

#define LED         6

//NRF24l01
#define CE          9  //Toggle between transmit (TX), receive (RX), standby, and power-down mode
#define CSN         10 //SPI chip select 

#define TRANSMIT_DELAY 50  

// Interrupt flags
volatile bool LOCK_FLAG = false;
volatile bool UNLOCK_FLAG = false;
volatile bool SENSITIVITY_FLAG = false;
volatile bool SOUND_FLAG = false;

//Constants
const char secret[30] = "77da4ba6-fdf2-11e7-8be5-0ed5f";
const uint64_t pipe = 0xE8E8F0F0E1LL;
const uint8_t lock_msg = 0;
const uint8_t unlock_msg = 1;
const uint8_t sensitivity_msg = 2; 
const uint8_t sound_msg = 3;

RF24 radio(CE, CSN);

void setup(void) {
    Serial.begin(115200);

    initializePins();
    attachInterrupts();

    radio.begin();
    radio.setPayloadSize(31);
    radio.openWritingPipe(pipe);
}
    
void loop(void) {

    char msg[31];
    memcpy(msg, secret, sizeof(secret)); 

    if(LOCK_FLAG){
        msg[30] = lock_msg;
        radio.write(msg, sizeof(msg)); 
        while(!digitalRead(LOCK)){
            radio.write(msg, sizeof(msg)); 
            delay(TRANSMIT_DELAY);
        }   
    }

    if(UNLOCK_FLAG){
        msg[30] = unlock_msg;
        radio.write(msg, sizeof(msg));
        while(!digitalRead(LOCK)){
            radio.write(msg, sizeof(msg)); 
            delay(TRANSMIT_DELAY);    
        }
    }

    if(SENSITIVITY_FLAG){
        msg[30] = sensitivity_msg;
        radio.write(msg, sizeof(msg));
        while(!digitalRead(LOCK)){
            radio.write(msg, sizeof(msg)); 
            delay(TRANSMIT_DELAY);    
        }
    }

    if(SOUND_FLAG){
        msg[30] = sound_msg;
        radio.write(msg, sizeof(msg));
        while(!digitalRead(LOCK)){
            radio.write(msg, sizeof(msg)); 
            delay(TRANSMIT_DELAY);   
        }
    }

    delay(10);
}

void initializePins(){
    pinMode(LOCK, INPUT_PULLUP);
    pinMode(UNLOCK, INPUT_PULLUP);
    pinMode(SENSITIVITY, INPUT_PULLUP);
    pinMode(SOUND, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
}

void attachInterrupts(){
    attachInterrupt(digitalPinToInterrupt(LOCK), lock, FALLING);
    attachInterrupt(digitalPinToInterrupt(UNLOCK), unlock, FALLING);
    attachInterrupt(digitalPinToInterrupt(SENSITIVITY), sensitivity, FALLING);
    attachInterrupt(digitalPinToInterrupt(SOUND), sound, FALLING);
}

void lock(){
    LOCK_FLAG = true;
}

void unlock(){
    UNLOCK_FLAG = true;
}

void sensitivity(){
    SENSITIVITY_FLAG = true;
}

void sound(){
    SOUND_FLAG = true;
}