#include <iostream>
#include "../headers/ArduinoHandler.h"

int main() {
    ArduinoHandler arduinoHandler("\\\\.\\COM4");
    long values[6] = {12345678, 12345678, 12345678, 12345678, 12345678, 12345678};
    arduinoHandler.queuePositions(values);
}