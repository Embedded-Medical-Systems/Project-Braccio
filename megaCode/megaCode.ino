#include <SPI.h>

struct Cart {
  const byte slaveSelect;
  const byte linPotPin;
  const byte rot1Pin;
  const byte rot2Pin;
  const float kpTrans;
  const float kdTrans;
  const float kpRot;
  const float kdRot;
  float prevErrorTrans;
  float prevErrorRot;
};

const float R = 1;

const float linPotFact = 100 / 1023.f;

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
  pinMode(cart.linPotPin, OUTPUT);
  pinMode(cart.rot1Pin, OUTPUT);
  pinMode(cart.rot2Pin, OUTPUT);
}

//speed should be between -1.0 --> 1.0
void setMotorSpeed(byte slaveSelect, float transSpeed, float rotSpeed) {
  byte directions = ((transSpeed >= 0) << 1) | (rotSpeed >= 0);
  byte transSpeedByte = min(abs(transSpeed) * 255, 255);
  byte rotSpeedByte = min(abs(rotSpeed) * 255, 255);
  digitalWrite(slaveSelect, LOW);
  SPI.transfer(directions);
  SPI.transfer(transSpeedByte);
  SPI.transfer(rotSpeedByte);
  digitalWrite(slaveSelect, HIGH);
}

float linearControl(float dist, Cart& cart) {
  float error = dist - analogRead(cart.linPotPin) * linPotFact;

  float proportional = error * cart.kpTrans;
  if(cart.prevErrorTrans == 0){
    cart.prevErrorTrans = error;
  }
  float derivative = (error - cart.prevErrorTrans) * cart.kdTrans;
  cart.prevErrorTrans = error;

  return proportional + derivative;

}

float rotationControl(float theta, Cart& cart) {
  float currentTheta = readPot(cart.rot1Pin, cart.rot2Pin);
  float error = theta - currentTheta;

  float proportional = error * cart.kpRot;
  if(cart.prevErrorRot == 0){
    cart.prevErrorRot = error;
  }
  float derivative = (error - cart.prevErrorRot) * cart.kdRot;
  cart.prevErrorRot = error;

  return proportional + derivative;
}

float readPot(const int potPin1, const int potPin2){
  return 0;
}

void getControllerPosition(float& x, float& y, float& z) {
  //read vive positions
}

void physicsModel(float x, float y, float z, float& d1, float& d2, float& theta) {
  theta = atan(y / x);
  float sqrtXY = sqrt(x * x + y * y);
  d2 = z - sqrt(R * R - (R - sqrtXY) * (R - sqrtXY));
  d1 =  d2 + R * acos((R - sqrtXY) / R);
}

void setup() {
  // put your setup code here, to run once:
  setCartPins(leftFrontCart);
  setCartPins(leftBackCart);
  setCartPins(rightFrontCart);
  setCartPins(rightBackCart);
}

void loop() {
  //TODO get two (X,Y,Z) positions from vive
  //TODO put them both through physics model
  float leftd1, rightd1, leftd2, rightd2, leftTheta, rightTheta;

  float leftFrontLin = linearControl(leftd1,leftFrontCart);
  float leftBackLin = linearControl(leftd2,leftBackCart);
  float leftFrontRot = rotationControl(leftTheta, leftFrontCart);
  float leftBackRot = rotationControl(leftTheta, leftBackCart);

  float rightFrontLin = linearControl(rightd1,rightFrontCart);
  float rightBackLin = linearControl(rightd2,leftBackCart);
  float rightFrontRot = rotationControl(rightTheta, rightFrontCart);
  float rightBackRot = rotationControl(rightTheta, leftBackCart);
  
  setMotorSpeed(leftFrontCart.slaveSelect, leftFrontLin, leftFrontRot);
  setMotorSpeed(leftBackCart.slaveSelect, leftBackLin, leftBackRot);
  setMotorSpeed(rightFrontCart.slaveSelect, rightFrontLin, rightFrontRot);
  setMotorSpeed(leftBackCart.slaveSelect, rightBackLin, rightBackRot);

}
