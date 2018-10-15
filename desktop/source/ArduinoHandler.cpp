//
// Created by Walter Raftus on 10/3/2018.
//

#include "../headers/ArduinoHandler.h"

ArduinoHandler::ArduinoHandler(char *port, bool debug) {
    handler = CreateFileA(static_cast<LPCSTR>(port),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                0,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                0);

    DCB serialParameters = {0};
    GetCommState(handler, &serialParameters);
    serialParameters.BaudRate = CBR_9600;
    serialParameters.ByteSize = 8;
    serialParameters.StopBits = ONESTOPBIT;
    serialParameters.Parity = NOPARITY;
    serialParameters.fDtrControl = DTR_CONTROL_ENABLE;
    SetCommState(handler, &serialParameters);
    PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);

    Sleep(1500);

    dataWaiting = false;
    closeThread = false;
    readingPots = false;
    writingPots = false;

    potValues = new byte[sizeOfData * numPots];
    memset((void*)potValues, 0, sizeOfData * numPots);
    std::thread (staticGetValues, this).detach();

    this->debug = debug;
}

ArduinoHandler::~ArduinoHandler() {
    while(!positionQueue.empty()){
        delete [] positionQueue.front();
        positionQueue.pop();
    }
    delete [] potValues;
    closeThread = true;
    CloseHandle(handler);
}

//input array should be six longs, in the order of leftd1, rightd1, lelftd2, rightd2, leftTheta, rightTheta
void ArduinoHandler::queuePositions(long long *values) {
    byte *bufferedValue = new byte[sizeOfData * numSetPoints];

    for(int i = 0; i < numSetPoints; i++){
        for(int j = 0; j < sizeOfData; j++) {
            bufferedValue[i * sizeOfData + j] = (byte)(values[i] & (0xff));
            values[i] = values[i] >> 8;
        }
    }
    positionQueue.push(bufferedValue);

    if(!dataWaiting)
        sendValues(false);
}

void ArduinoHandler::getPots(long long *values) {
    while(writingPots);
    readingPots = true;
    for(int i = 0; i < numPots; i++){
        long long value = 0;
        for(int j = 0; j < sizeOfData; j++){
            value |= (long long)potValues[i * sizeOfData + j] << (8 * j);
        }
        values[i] = value;
    }
    readingPots = false;
}

void ArduinoHandler::sendValues(bool fromThread) {
    if(!positionQueue.empty()) {
        if(debug) std::cout << "Sending Values" << std::endl;
        OVERLAPPED oWrite = {0};
        oWrite.hEvent = CreateEvent(nullptr, true, false, nullptr);
        DWORD bytesWritten;

        byte *buffer = positionQueue.front();
        positionQueue.pop();
        if(!WriteFile(handler, (void*) buffer, sizeOfData * numSetPoints, &bytesWritten, &oWrite)){
            if(GetLastError() != ERROR_IO_PENDING){
                if(debug) std::cout << "Write Failed 1";
            }
            else {
                if(!GetOverlappedResult(handler, &oWrite, &bytesWritten, true)) {
                    if(debug) std::cout << "Write Failed 2" << std::endl;
                }
                else if(debug)
                    std::cout << "Write Complete After Waiting " << bytesWritten << std::endl;
            }
        }
        else if(debug)
            std::cout << "Write Completed Immediately " << bytesWritten << std::endl;

        CloseHandle(oWrite.hEvent);
        delete buffer;
    }

    dataWaiting = true;
    if(fromThread && !closeThread)
        getValues();
    else if(fromThread)
        terminate();
}

void ArduinoHandler::getValues() {
    while (readingPots);
    if(debug) std::cout << "Getting Values" << std::endl;
    writingPots = true;

    DWORD bytesRead;
    OVERLAPPED oReader = {0};
    oReader.hEvent = CreateEvent(nullptr, true, false, nullptr);

    if(!ReadFile(handler, potValues, sizeOfData * numPots, &bytesRead, &oReader)) {
        if(GetLastError() != ERROR_IO_PENDING) {
            if(debug)  std::cout << "Read Failed 1" << std::endl;
        }
        else{
            if(debug) std::cout << "Waiting For Read" << std::endl;
            switch(WaitForSingleObject(oReader.hEvent, INFINITE)) {
                case WAIT_OBJECT_0:
                    if(!GetOverlappedResult(handler, &oReader, &bytesRead, false)) {
                        if(debug) std::cout << "Read Failed 2" << std::endl;
                    }
                    else if(debug)
                        std::cout << "Read Complete After Waiting " << bytesRead << std::endl;
                    break;
                default:
                    if(debug) std::cout << "Issue with OVERLAPPED's handle";
                    break;
            }
        }
    }
    else if(debug)
        std::cout << "Read Completed Immediately " << bytesRead << std::endl;

    writingPots = false;
    dataWaiting = false;
    CloseHandle(oReader.hEvent);
    if(!closeThread)
        sendValues(true);
    else
        terminate();
}

void ArduinoHandler::staticGetValues(ArduinoHandler* arduinoHandler) {
    Sleep(100);
    arduinoHandler->getValues();
}