// moto-alarm Reciever

#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Push buttons
#define LOCK        2
#define UNLOCK      3
#define SENSITIVITY A6 // 4
#define SOUND       A7 // 5

#define LED         6

//NRF24l01
#define CE          9  //Toggle between transmit (TX), receive (RX), standby, and power-down mode
#define CSN         10 //SPI chip select 

#define TRANSMIT_DELAY 50  
#define PAYLOAD_SIZE   31

// Interrupt flags
volatile bool LOCK_FLAG = false;
volatile bool UNLOCK_FLAG = false;
volatile bool SENSITIVITY_FLAG = false;
volatile bool SOUND_FLAG = false;

//Constants
const char secret[30] = "77da4ba6-fdf2-11e7-8be5-0ed5ff";
const uint64_t pipe = 0xE8E8F0F0E1LL;
const uint8_t lock_code = '0';
const uint8_t unlock_code = '1';
const uint8_t sensitivity_code = '2'; 
const uint8_t sound_code = '3';

RF24 radio(CE, CSN);

void setup(void) {
    Serial.begin(115200);

    initializePins();
    attachInterrupts();

    radio.begin();
    radio.setPayloadSize(PAYLOAD_SIZE);
    radio.openWritingPipe(pipe);
}
    
void loop(void) {
    if(LOCK_FLAG){
        processKeyPress(LOCK, lock_code); 
        LOCK_FLAG = false;
    }

    if(UNLOCK_FLAG){
        processKeyPress(UNLOCK, unlock_code);
        UNLOCK_FLAG = false;
    }

    if(SENSITIVITY_FLAG){
        processKeyPress(SENSITIVITY, sensitivity_code);
        SENSITIVITY_FLAG = false;
    }

    if(SOUND_FLAG){
        processKeyPress(SOUND, sound_code);
        SOUND_FLAG = false;
    }

    delay(10);
}

void processKeyPress(uint8_t key_pin, uint8_t key_code){
    Serial.write(key_code);
    Serial.println(" pressed");
    
    digitalWrite(LED, LOW);

    char msg[PAYLOAD_SIZE];
    memcpy(msg, secret, sizeof(secret)); 
    msg[30] = key_code;

    radio.write(msg, sizeof(msg)); 
    while(!digitalRead(key_pin)){
        radio.write(msg, sizeof(msg)); 
        delay(TRANSMIT_DELAY);
    }

    digitalWrite(LED, HIGH);
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
    Serial.println("Sensitivity");
    SENSITIVITY_FLAG = true;
}

void sound(){
    SOUND_FLAG = true;
}