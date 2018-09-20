#include <EEPROM.h>

#include <SPI.h>

byte slaveSelect = A3;
byte transForward1 = 4;
byte transForward2 = 6;
byte transBack1 = 3;
byte transBack2 = 5;
byte rotForward1 = 8;
byte rotForward2 = 10;
byte rotBack1 = 7;
byte rotBack2 = 9;

boolean lastSlaveSelect = HIGH;
const byte DIRECTIONS = 0;
const byte TRANS = 1;
const byte ROT = 2;
const byte NUM_BYTES = 3;
byte currentByte = 0;

byte controlBytes[NUM_BYTES] = {3, 0  , 0};
char isLedOn = false;
byte pos;

void setup() {
  Serial.begin(9600);
  pinMode(transForward1, OUTPUT);
  pinMode(transForward2, OUTPUT);
  pinMode(transBack1, OUTPUT);
  pinMode(transBack2, OUTPUT);
  pinMode(rotForward1, OUTPUT);
  pinMode(rotForward2, OUTPUT);
  pinMode(rotBack1, OUTPUT);
  pinMode(rotBack2, OUTPUT);
  
  SPCR |= (1 << SPE); //turns on SPI
  SPCR |= _BV(SPIE); //attaches interupt
  pinMode(slaveSelect, INPUT);
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, INPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

boolean leds[3] = {0, 0, 0};
byte currentLED = 0;

ISR (SPI_STC_vect)
{
  if(!digitalRead(slaveSelect) && currentLED < 3){
    leds[currentLED++] = SPDR;
  }
}

void loop() {
  if(digitalRead(slaveSelect)){
    //if its the first cycle with a high slave select
    if(lastSlaveSelect == LOW){
      lastSlaveSelect = HIGH;
      pinMode(MISO, INPUT);
      currentLED = 0;
    }
    for(int i = 0; i < 3; i++){
      digitalWrite(2 + i, leds[i] ? HIGH : LOW);
    }
  }
  
  else {    
    if(lastSlaveSelect == HIGH){
      lastSlaveSelect = LOW;
      pinMode(MISO, OUTPUT);
    }
  }

}
