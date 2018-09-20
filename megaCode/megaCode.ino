#include <SPI.h>

struct Cart {
  byte slaveSelect;
  byte linPotPin;
  byte rot1Pin;
  byte rot2Pin;
  float kp;
  float kd;
  float prevError;
};

const float R = 1;

const float linPotFact = 100 / 1023.f;

const Cart leftFrontCart = {
  .slaveSelect = 2,
  .linPotPin = A0,
  .rot1Pin = A1,
  .rot2Pin = A2
};

const Cart rightFrontCart = {
  .slaveSelect = 3,
  .linPotPin = A3,
  .rot1Pin = A4,
  .rot2Pin = A5
};

const Cart leftBackCart = {
  .slaveSelect = 4,
  .linPotPin = A6,
  .rot1Pin = A7,
  .rot2Pin = A8
};

const Cart rightBackCart = {
  .slaveSelect = 5,
  .linPotPin = A9,
  .rot1Pin = A10,
  .rot2Pin = A11
};

void setCartPins(Cart cart) {
  pinMode(cart.slaveSelect, OUTPUT);
  digitalWrite(cart.slaveSelect, HIGH);
  pinMode(cart.linPotPin, OUTPUT);
  pinMode(cart.rot1Pin, OUTPUT);
  pinMode(cart.rot2Pin, OUTPUT);
}

//speed should be between -1.0 --> 1.0
void setMotorSpeed(byte slaveSelect, float transSpeed, float rotSpeed) {
  unsigned char directions = ((transSpeed >= 0) << 1) | (rotSpeed >= 0);
  unsigned char transSpeedByte = min(abs(transSpeed) * 255, 255);
  unsigned char rotSpeedByte = min(abs(rotSpeed) * 255, 255);
  digitalWrite(slaveSelect, LOW);
  delay(20);
  SPI.transfer(directions);
  delay(20);
  SPI.transfer(transSpeedByte);
  delay(20);
  SPI.transfer(rotSpeedByte);
  delay(20);
  digitalWrite(slaveSelect, HIGH);
  
  Serial.println(rotSpeedByte);
}

void linearControl(float dist, Cart cart) {
  //TODO do control loops/ run motors
  float error = dist - analogRead(cart.linPotPin) * linPotFact;

  float proportional = error * cart.kp;
  float derivative = (error - cart.prevError) * cart.kd;

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
  Serial.begin(9600);
  setCartPins(leftFrontCart);
  setCartPins(leftBackCart);
  setCartPins(rightFrontCart);
  setCartPins(rightBackCart);
  SPI.begin();
}

void loop() {
  //TODO get two (X,Y,Z) positions from vive
  //TODO put them both through physics model
  //TODO use control loop on physics model

}
