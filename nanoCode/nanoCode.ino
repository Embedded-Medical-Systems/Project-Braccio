#include <SPI.h>

byte slaveSelect = A7;
byte transForward1 = 4;
byte transForward2 = 6;
byte transBack1 = 3;
byte transBack2 = 5;
byte rotForward1 = 8;
byte rotForward2 = 10;
byte rotBack1 = 7;
byte rotBack2 = 9;

boolean lastSlaveSelect = HIGH;
const byte DIRECTIONS_INDEX = 0;
const byte TRANS_INDEX = 1;
const byte ROT_INDEX = 2;
const byte FINISHED_INDEX = 3;

volatile byte directions = 3;
volatile byte transSpeed = 0;
volatile byte rotSpeed = 0;

void setup() {
  pinMode(transForward1, OUTPUT);
  pinMode(transForward2, OUTPUT);
  pinMode(transBack1, OUTPUT);
  pinMode(transBack2, OUTPUT);
  pinMode(rotForward1, OUTPUT);
  pinMode(rotForward2, OUTPUT);
  pinMode(rotBack1, OUTPUT);
  pinMode(rotBack2, OUTPUT);
  
  SPCR |= (1 << SPE); //turns on SPI
  pinMode(slaveSelect, INPUT);
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, INPUT);
   
}

byte requestByte(byte index){
  SPDR = index;
  while(!(SPSR & (1 << SPIF)));
  return SPDR;
}

void loop() {
  if(digitalRead(slaveSelect) == HIGH){
    //if its the first cycle with a high slave select
    if(lastSlaveSelect == LOW){
      lastSlaveSelect = HIGH;
      pinMode(MISO, INPUT);
    }
    //set motor speeds & directions
    digitalWrite(transForward1, directions & 2 ? HIGH : LOW);
    digitalWrite(transForward2, directions & 2 ? HIGH : LOW);
    digitalWrite(transBack1, directions & 2 ? LOW : HIGH);
    digitalWrite(transBack2, directions & 2 ? LOW : HIGH);
    analogWrite(directions & 2 ? transForward2 : transBack2, transSpeed & 0xff);
    
    digitalWrite(rotForward1, directions & 1 ? HIGH : LOW);
    digitalWrite(rotForward2, directions & 1 ? HIGH : LOW);
    digitalWrite(rotBack1, directions & 1 ? LOW : HIGH);
    digitalWrite(rotBack2, directions & 1 ? LOW : HIGH);
    analogWrite(directions & 1 ? rotForward2 : rotBack2, rotSpeed & 0xff);
  }
  
  else {
    if(lastSlaveSelect == HIGH){
      lastSlaveSelect = LOW;
      directions = requestByte(DIRECTIONS_INDEX);
      transSpeed = requestByte(TRANS_INDEX);
      rotSpeed = requestByte(ROT_INDEX);
      requestByte(FINISHED_INDEX);
    }
  }
}
