using System;
using System.IO.Ports;

namespace vive
{
    class ArduinoHandler
    {
        private int baudRate;
        private String comPort;
        private SerialPort port;

        public ArduinoHandler(int baudRate, String comPort)
        {
            port = new SerialPort();
            port.BaudRate = this.baudRate = baudRate;
            port.PortName = this.comPort = comPort;
            port.Open();
        }

        public void sendValue(string value)
        {
            port.Write(value);
        }

    }
}
