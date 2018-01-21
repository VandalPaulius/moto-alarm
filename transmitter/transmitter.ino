// moto-alarm Reciever
//Used example from http://forum.arduino.cc/index.php?topic=421081

#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define DEBUG

// Push buttons
#define LOCK        4
#define UNLOCK      5
#define SENSITIVITY 6
#define SOUND       7

#define LED         8

//NRF24l01
#define CE          9  //Toggle between transmit (TX), receive (RX), standby, and power-down mode
#define CSN         10 //SPI chip select 

#define TRANSMIT_DELAY 140
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
    #ifdef DEBUG
        Serial.begin(115200);
        printf_begin();
    #endif

    initializePins();
    initializeRadio();
    attachInterrupts();
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

void initializePins(){
    pinMode(LOCK, INPUT_PULLUP);
    pinMode(UNLOCK, INPUT_PULLUP);
    pinMode(SENSITIVITY, INPUT_PULLUP);
    pinMode(SOUND, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
}

void initializeRadio(){
    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(3,5); // delay, count
    radio.setPayloadSize(PAYLOAD_SIZE);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openWritingPipe(pipe);
    radio.stopListening();
}

void attachInterrupts(){
    pciSetup(LOCK);
    pciSetup(UNLOCK);
    pciSetup(SENSITIVITY);
    pciSetup(SOUND);
}

void processKeyPress(uint8_t key_pin, uint8_t key_code){
    #ifdef DEBUG   
        printf("%c pressed\r\n", key_code);
    #endif

    digitalWrite(LED, LOW);

    char msg[PAYLOAD_SIZE + 1];
    memcpy(msg, secret, PAYLOAD_SIZE - 1); 
    msg[PAYLOAD_SIZE - 1] = key_code;
    msg[PAYLOAD_SIZE] = '\0';

    bool test = radio.write(msg, PAYLOAD_SIZE); 

    #ifdef DEBUG
        printf("%s %i\r\n", msg, test);
    #endif

    delay(TRANSMIT_DELAY);

    while(!digitalRead(key_pin)){
        test = radio.write(msg, PAYLOAD_SIZE); 

        #ifdef DEBUG
            printf("%s %i\r\n", msg, test);
        #endif
        
        delay(TRANSMIT_DELAY);
    }

    digitalWrite(LED, HIGH);
}

void pciSetup(byte pin){
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

ISR (PCINT2_vect) {// handle pin change interrupt for D0 to D7 here
    if(!digitalRead(LOCK)){
        LOCK_FLAG = true;
    }     
    else if(!digitalRead(UNLOCK)){
        UNLOCK_FLAG = true;
    }
    else if(!digitalRead(SENSITIVITY)){
        SENSITIVITY_FLAG = true;
    }
    else if(!digitalRead(SOUND)){
        SOUND_FLAG = true;
    }
}  

