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
        volatile private Queue<byte[]> potValues;
        volatile private bool dataWaiting = false;

        public const byte sizeOfData = 8;
        public const byte numSetPoints = 6;
        public const byte numPots = 8;

        public ArduinoHandler(int baudRate, String comPort)
        {
            setPoints = new Queue<byte[]>();
            port = new SerialPort();
            port.BaudRate = this.baudRate = baudRate;
            port.PortName = this.comPort = comPort;
            port.Open();
        }

        private void sendValues()
        {
            if(setPoints.Count > 0)
            {
                port.Write(setPoints.Dequeue(), 0, sizeOfData * numSetPoints);
                dataWaiting = true;
            }
        }

        //input array should be six longs, in the order of leftd1, rightd1, lelftd2, rightd2, leftTheta, rightTheta
        public void queueValues(long[] values)
        {
            byte[] bufferValue = new byte[sizeOfData * numSetPoints];
            
            for(int j = 0; j < numSetPoints; j++)
            {
                for(int i = 0; i < sizeOfData; i++)
                {
                    bufferValue[j * sizeOfData + i] = (byte)(values[j] & (0xff));
                    values[j] >>= 8;
                }     
            }
            setPoints.Enqueue(bufferValue);

            if(!dataWaiting)
                sendValues();
        }
        
        private void dataRecievedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            byte[] buffer = new byte[numPots * sizeOfData];
            sp.Read(buffer, 0, numPots * sizeOfData);
            potValues.Enqueue(buffer);
            dataWaiting = false;
            sendValues();
        }
    }
}
