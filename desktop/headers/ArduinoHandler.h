//
// Created by Walter Raftus on 10/3/2018.
//

#ifndef DESKTOP_ARDUINOHANDLER_H
#define DESKTOP_ARDUINOHANDLER_H

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <thread>

#include <iostream>
#include <bitset>

class ArduinoHandler{
private:
    HANDLE handler;

    std::queue<byte*> positionQueue;
    byte* potValues;

    volatile bool dataWaiting;
    volatile bool closeThread;
    volatile bool readingPots;
    volatile bool writingPots;

    void getValues();
    static void staticGetValues(ArduinoHandler* arduinoHandler);
    void sendValues(bool fromThread);

    volatile bool debug;

public:
    static const byte sizeOfData = 8;
    static const byte numSetPoints = 6;
    const byte numPots = 8;

    ArduinoHandler(char *port, bool debug);
    ~ArduinoHandler();

    void queuePositions(long long *values);
    void getPots(long long *values);

};

#endif //DESKTOP_ARDUINOHANDLER_H
