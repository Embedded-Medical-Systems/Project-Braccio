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

byte controlBytes[NUM_BYTES] = {3, 0, 0};

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

ISR (SPI_STC_vect)
{
  if(!digitalRead(slaveSelect) && currentByte < NUM_BYTES){
    controlBytes[currentByte] = SPDR;
  }
}

void loop() {
  if(digitalRead(slaveSelect)){
    //if its the first cycle with a high slave select
    if(lastSlaveSelect == LOW){
      lastSlaveSelect = HIGH;
      pinMode(MISO, INPUT);
      currentByte = 0;
    }
    
    digitalWrite(transForward1, controlBytes[DIRECTIONS] & 2 ? HIGH : LOW);
    digitalWrite(transBack1, controlBytes[DIRECTIONS] & 2 ? LOW : HIGH);
    digitalWrite(controlBytes[DIRECTIONS] & 2 ? transBack2 : transForward2, LOW);
    analogWrite(controlBytes[DIRECTIONS] & 2 ? transForward2 : transBack2, controlBytes[TRANS] & 0xff);
    
    digitalWrite(rotForward1, controlBytes[DIRECTIONS] & 1 ? HIGH : LOW);
    digitalWrite(rotBack1, controlBytes[DIRECTIONS] & 1 ? LOW : HIGH);
    digitalWrite(controlBytes[DIRECTIONS] & 1 ? rotBack2 : rotForward2, LOW);
    analogWrite(controlBytes[DIRECTIONS] & 1 ? rotForward2 : rotBack2, controlBytes[ROT] & 0xff);
  }
  
  else {    
    if(lastSlaveSelect == HIGH){
      lastSlaveSelect = LOW;
      pinMode(MISO, OUTPUT);
    }
  }

}
