#include <SPI.h>

int slaveSelect A7;

int readCounter = -1;
const byte DIRECTIONS_INDEX = 0;
const byte TRANS_INDEX = 1;
const byte ROT_INDEX = 2;

volatile int directions = 3;
volatile int transSpeed = 0;
volatile int rotSPeed = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(slaveSelect, OUTPUT);
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
  // put your main code here, to run repeatedly:
}
