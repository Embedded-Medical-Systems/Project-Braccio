#include <SPI.h>

byte slaveSelect A7;
byte transForward1 = D4;
byte transForward2 = D6;
byte transBack1 = D3;
byte transBack2 = D5;
byte rotForward1 = D8;
byte rotForward2 = D10;
byte rotBack1 = D7;
byte rotBack2 = D9;

int readCounter = -1;
const byte DIRECTIONS_INDEX = 0;
const byte TRANS_INDEX = 1;
const byte ROT_INDEX = 2;

volatile int directions = 3;
volatile int transSpeed = 0;
volatile int rotSPeed = 0;

void setup() {
  pinMode(slaveSelect, INPUT);
  pinMode(transForward1, OUTPUT);
  pinMode(transForward2, OUTPUT);
  pinMode(transBack1, OUTPUT);
  pinMode(transBack2, OUTPUT);
  pinMode(rotForward1, OUTPUT);
  pinMode(rotForward2, OUTPUT);
  pinMode(rotBack1, OUTPUT)
  pinMode(rotBack2, OUTPUT);
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPCR |= _BV(SPE);
  SPI.attachInterrupt();
}

ISR (SPI_STC_vect) {
  if (++readCounter == DIRECTIONS_INDEX){
    directions = SPDR;
  }
  else if (readcounter == TRANS_INDEX){
    transSpeed = SPDR;
  }
  else {
    rotSpeed = SPDR;
    readCounter =- -1;
  }
  
}

void loop() {
  digitalWrite(transFront1, direction & 2 ? HIGH : LOW);
  digitalWrite(transFront2, direction & 2 ? HIGH : LOW);

  digitalWrite(transBack1, direction & 2 ? LOW : HIGH);
  digitalWrite(transBack2, direction & 2 ? LOW : HIGH);

  analogWrite(direction & 2 ? transBack1 : transBack2, transSpeed & 0xFF);

  digitalWrite(rotFront1, direction & 1 ? HIGH : LOW);
  digitalWrite(rotFront2, direction & 1 ? HIGH : LOW);

  digitalWrite(rotBack1, direction & 1 ? LOW : HIGH);
  digitalWrite(rotBack2, direction & 1 ? LOW : HIGH);

  analogWrite(direction & 1 ? rotBack1 : rotBack2, rotSpeed & 0xFF);
}
