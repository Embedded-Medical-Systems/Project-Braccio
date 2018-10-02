using System;
using System.IO.Ports;

namespace vive
{
    class ArduinoHandler
    {
        private int baudRate;
        private String comPort;
        private SerialPort port;

        public const byte sizeOfData = 8;
        public const byte numData = 6;

        public ArduinoHandler(int baudRate, String comPort)
        {
            port = new SerialPort();
            port.BaudRate = this.baudRate = baudRate;
            port.PortName = this.comPort = comPort;
            port.Open();
        }

        //input array should be six longs, in the order of leftd1, rightd1, lelftd2, rightd2, leftTheta, rightTheta
        public void sendValues(long[] values)
        {
            byte[] bufferValue = new byte[sizeOfData * numData];
            
            for(int j = 0; j < numData; j++){
                for(int i = 0; i < sizeOfData; i++) {
                    bufferValue[j * sizeOfData + i] = (byte)(value & (0xff));
                    value >>= 8;
                }     
            }

            port.Write(bufferValue, 0, 8);
            while(true){
                String a = port.ReadExisting();
                Console.Write(a);
            }
        }

    }
}
