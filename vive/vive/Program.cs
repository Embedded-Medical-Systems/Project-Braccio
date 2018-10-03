using System;

namespace vive
{
    class Program
    {

        static void Main(string[] args)
        {
            ArduinoHandler handler = new ArduinoHandler(9600, "COM4");
        }

    }
}
