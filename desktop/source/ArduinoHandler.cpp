//
// Created by Walter Raftus on 10/3/2018.
//

#include "../headers/ArduinoHandler.h"

ArduinoHandler::ArduinoHandler(char *port) {
    handler = CreateFileA(static_cast<LPCSTR>(port),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

    DCB serialParameters = {0};
    GetCommState(handler, &serialParameters);
    serialParameters.BaudRate = CBR_9600;
    serialParameters.ByteSize = 8;
    serialParameters.StopBits = ONESTOPBIT;
    serialParameters.Parity = NOPARITY;
    serialParameters.fDtrControl = DTR_CONTROL_DISABLE;
    SetCommState(handler, &serialParameters);
    PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);

    dataWaiting = false;
}

ArduinoHandler::~ArduinoHandler() {
    while(!positionQueue.empty()){
        delete positionQueue.front();
        positionQueue.pop();
        dataWaiting = true;
    }
    CloseHandle(handler);
}

//input array should be six longs, in the order of leftd1, rightd1, lelftd2, rightd2, leftTheta, rightTheta
void ArduinoHandler::queuePositions(long *values) {
    byte *bufferedValue = new byte[sizeOfData * numSetPoints];

    for(int i = 0; i < numSetPoints; i++){
        for(int j = 0; j < sizeOfData; j++) {
            bufferedValue[i * sizeOfData + j] = (byte)(values[i] & (0xff));
            values[i] = values[i] >> 8;
        }
    }
    positionQueue.push(bufferedValue);

    if(!dataWaiting)
        sendValues();
}

void ArduinoHandler::sendValues() {
    if(!positionQueue.empty()) {
        byte *buffer = positionQueue.front();
        positionQueue.pop();
        WriteFile(handler, (void*) buffer, sizeOfData * numSetPoints, nullptr, nullptr);
        delete buffer;
    }

    std::cout << "RecievedValues" << std::endl;
    for(int j = 0; j < 8; j++) {
        byte buffer[sizeOfData];
        while(!ReadFile(handler, buffer, 8, nullptr, NULL));
        for (int i = 0; i < 8; i++) {
            std::bitset<8> x(buffer[i]);
            std::cout << x;
        }
        std::cout << std::endl;
    }

}