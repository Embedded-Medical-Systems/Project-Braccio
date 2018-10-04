//
// Created by Walter Raftus on 10/3/2018.
//

#ifndef DESKTOP_ARDUINOHANDLER_H
#define DESKTOP_ARDUINOHANDLER_H

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include <iostream>
#include <bitset>

class ArduinoHandler{
private:
    HANDLE handler;

    std::queue<byte*> positionQueue;
    //std::queue<byte*> potQueue;
    volatile bool dataWaiting;

    void sendValues();

public:
    static const byte sizeOfData = 8;
    static const byte numSetPoints = 6;
    //const byte numPots = 8;

    ArduinoHandler(char *port);
    ~ArduinoHandler();

    void queuePositions(long *values);
    //void getPots(byte *values);

};

#endif //DESKTOP_ARDUINOHANDLER_H
