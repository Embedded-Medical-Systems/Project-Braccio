using System;
using System.IO.Ports;
using System.Collections.Generic;

namespace vive
{
    class ArduinoHandler
    {
        private int baudRate;
        private String comPort;
        private SerialPort port;

        volatile private Queue<byte[]> setPoints;
        volatile bool dataWaiting = false;

        public const byte sizeOfData = 8;
        public const byte numData = 6;

        public ArduinoHandler(int baudRate, String comPort)
        {
            setPoints = new Queue<byte[]>();
            port = new SerialPort();
            port.BaudRate = this.baudRate = baudRate;
            port.PortName = this.comPort = comPort;
            port.Open();
        }

        private void sendValue()
        {
            if(setPoints.Count > 0)
            {
                port.Write(setPoints.Dequeue(), 0, sizeOfData * numData);
                dataWaiting = true;
            }
        }

        //input array should be six longs, in the order of leftd1, rightd1, lelftd2, rightd2, leftTheta, rightTheta
        public void queueValues(long[] values)
        {
            byte[] bufferValue = new byte[sizeOfData * numData];
            
            for(int j = 0; j < numData; j++)
            {
                for(int i = 0; i < sizeOfData; i++)
                {
                    bufferValue[j * sizeOfData + i] = (byte)(values[j] & (0xff));
                    values[j] >>= 8;
                }     
            }
            setPoints.Enqueue(bufferValue);

            if(!dataWaiting)
                sendValue();
        }                                                                                              
    }
}
