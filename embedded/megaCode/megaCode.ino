#include <SPI.h>

struct Cart {
  const byte slaveSelect;
  const byte linPotPin;
  const byte rot1Pin;
  const byte rot2Pin;
  const long long kpTrans;
  const long long kdTrans;
  const long long kpRot;
  const long long kdRot;
  long long prevErrorTrans;
  long long prevErrorRot;
};

const byte sizeOfUnit = 8;
const byte numSetPoints = 6;
const byte numPots = 8;

const long long scaledUnit = 10000000;
const long long linPotFact = (100 * scaledUnit) / 1023;

long long leftd1, rightd1, leftd2, rightd2, leftTheta, rightTheta;

long long readPot(const int potPin1, const int potPin2){
  return 0;
}

Cart leftFrontCart = {
  .slaveSelect = 2,
  .linPotPin = A0,
  .rot1Pin = A1,
  .rot2Pin = A2,
  .kpTrans = 0,
  .kdTrans = 0,
  .kpRot = 0,
  .kdRot = 0,
  .prevErrorTrans = 0,
  .prevErrorRot = 0
};

Cart rightFrontCart = {
  .slaveSelect = 3,
  .linPotPin = A3,
  .rot1Pin = A4,
  .rot2Pin = A5,
  .kpTrans = 0,
  .kdTrans = 0,
  .kpRot = 0,
  .kdRot = 0,
  .prevErrorTrans = 0,
  .prevErrorRot = 0
};

Cart leftBackCart = {
  .slaveSelect = 4,
  .linPotPin = A6,
  .rot1Pin = A7,
  .rot2Pin = A8,
  .kpTrans = 0,
  .kdTrans = 0,
  .kpRot = 0,
  .kdRot = 0,
  .prevErrorTrans = 0,
  .prevErrorRot = 0
};

Cart rightBackCart = {
  .slaveSelect = 5,
  .linPotPin = A9,
  .rot1Pin = A10,
  .rot2Pin = A11,
  .kpTrans = 0,
  .kdTrans = 0,
  .kpRot = 0,
  .kdRot = 0,
  .prevErrorTrans = 0,
  .prevErrorRot = 0
};

void setCartPins(Cart cart) {
  pinMode(cart.slaveSelect, OUTPUT);
  digitalWrite(cart.slaveSelect, HIGH);
  pinMode(cart.linPotPin, INPUT);
  pinMode(cart.rot1Pin, INPUT);
  pinMode(cart.rot2Pin, INPUT);
}

//speed should be between -255 --> 255
void setMotorSpeed(byte slaveSelect, int transSpeed, int rotSpeed) {
  unsigned char directions = ((transSpeed >= 0) << 1) | (rotSpeed >= 0);
  unsigned char transSpeedByte = min(abs(transSpeed), 255);
  unsigned char rotSpeedByte = min(abs(rotSpeed), 255);
  digitalWrite(slaveSelect, LOW);
  delay(1);
  SPI.transfer(directions);
  delay(1);
  SPI.transfer(transSpeedByte);
  delay(1);
  SPI.transfer(rotSpeedByte);
  delay(1);
  digitalWrite(slaveSelect, HIGH);
}

short linearControl(long long dist, Cart& cart) {
  long long error = dist - analogRead(cart.linPotPin) * linPotFact;

  long long proportional = error * cart.kpTrans;
  if(cart.prevErrorTrans == 0){
    cart.prevErrorTrans = error;
  }
  long long derivative = (error - cart.prevErrorTrans) * cart.kdTrans;
  cart.prevErrorTrans = error;

  return (proportional + derivative) / scaledUnit; //TODO scale from -255 to 255

}

short rotationControl(long long theta, Cart& cart) {
  long long currentTheta = readPot(cart.rot1Pin, cart.rot2Pin);
  long long error = theta - currentTheta;

  long long proportional = error * cart.kpRot;
  if(cart.prevErrorRot == 0){
    cart.prevErrorRot = error;
  }
  long long derivative = (error - cart.prevErrorRot) * cart.kdRot;
  cart.prevErrorRot = error;

  return (proportional + derivative) / scaledUnit; //TODO scale from -255 to 255
}



void getDistances(long long& leftd1, long long& rightd1, long long& leftd2,
                  long long& rightd2, long long& leftTheta, long long& rightTheta) {
  leftd1 = 0;
  for(int i = 0; i < sizeOfUnit; i++){
    leftd1 += ((long long)Serial.read()) << 8 * i;
  }

  rightd1 = 0;
  for(int i = 0; i < sizeOfUnit; i++){
    rightd1 += ((long long)Serial.read()) << 8 * i;
  }

  leftd2 = 0;
  for(int i = 0; i < sizeOfUnit; i++){
    leftd2 += ((long long)Serial.read()) << 8 * i;
  }

  rightd2 = 0;
  for(int i = 0; i < sizeOfUnit; i++){
    rightd2 += ((long long)Serial.read()) << 8 * i;
  }

  leftTheta = 0;
  for(int i = 0; i < sizeOfUnit; i++){
    leftTheta += ((long long)Serial.read()) << 8 * i;
  }

  rightTheta = 0;
  for(int i = 0; i < sizeOfUnit; i++){
    rightTheta += ((long long)Serial.read()) << 8 * i;
  }

  sendPotValues();
}

void sendPotValues(){
  byte potValues[sizeOfUnit * numPots];
  byte counter = 0;
  
  long long currPot = analogRead(leftFrontCart.linPotPin) * linPotFact;
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }
  currPot = readPot(leftFrontCart.rot1Pin, leftFrontCart.rot2Pin);
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }

  currPot = analogRead(rightFrontCart.linPotPin) * linPotFact;
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }
  currPot = readPot(rightFrontCart.rot1Pin, rightFrontCart.rot2Pin);
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }

  currPot = analogRead(leftBackCart.linPotPin) * linPotFact;
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }
  currPot = readPot(leftBackCart.rot1Pin, leftBackCart.rot2Pin);
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }

  currPot = analogRead(rightBackCart.linPotPin) * linPotFact;
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }
  currPot = readPot(rightBackCart.rot1Pin, rightBackCart.rot2Pin);
  for(int i = 0; i < sizeOfUnit; i++){
    potValues[counter++] = currPot & 0xff;
    currPot >>= 8;
  }

  Serial.write(potValues, sizeOfUnit * 8);
}

//void physicsModel(float x, float y, float z, float& d1, float& d2, float& theta) {
//  theta = atan(y / x);
//  float sqrtXY = sqrt(x * x + y * y);
//  d2 = z - sqrt(R * R - (R - sqrtXY) * (R - sqrtXY));
//  d1 =  d2 + R * acos((R - sqrtXY) / R);
//}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setCartPins(leftFrontCart);
  setCartPins(leftBackCart);
  setCartPins(rightFrontCart);
  setCartPins(rightBackCart);
  SPI.begin();

  leftd1 = analogRead(leftFrontCart.linPotPin) * linPotFact;
  rightd1 = analogRead(rightFrontCart.linPotPin) * linPotFact;
  left21 = analogRead(leftBackCart.linPotPin) * linPotFact;
  rightd2 = analogRead(rightBackCart.linPotPin) * linPotFact;
  leftTheta = readPot(leftFrontCart.rot1Pin, leftFrontCart.rot2Pin);
  rightTheta = readPot(rightFrontCart.rot1Pin, rightFrontCart.rot2Pin);
}

void loop() {
  if(Serial.available()){
    getDistances(leftd1, rightd1, leftd2, rightd2, leftTheta, rightTheta);
  }

  short leftFrontLin = linearControl(leftd1,leftFrontCart);
  short leftBackLin = linearControl(leftd2,leftBackCart);
  short leftFrontRot = rotationControl(leftTheta, leftFrontCart);
  short leftBackRot = rotationControl(leftTheta, leftBackCart);

  short rightFrontLin = linearControl(rightd1,rightFrontCart);
  short rightBackLin = linearControl(rightd2,rightBackCart);
  short rightFrontRot = rotationControl(rightTheta, rightFrontCart);
  short rightBackRot = rotationControl(rightTheta, rightBackCart);
  
  setMotorSpeed(leftFrontCart.slaveSelect, leftFrontLin, leftFrontRot);
  setMotorSpeed(leftBackCart.slaveSelect, leftBackLin, leftBackRot);
  setMotorSpeed(rightFrontCart.slaveSelect, rightFrontLin, rightFrontRot);
  setMotorSpeed(leftBackCart.slaveSelect, rightBackLin, rightBackRot);

}
