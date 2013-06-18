using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Threading;

namespace Lab
{
   class Program
   {
      static void Main (String[] options)
      {
         Console.WriteLine("UART Ping");
         using (var port = new SerialPort("COM5", 57600, Parity.None, 8, StopBits.One))
         {
            port.Open();
            port.ReadExisting();
            for (; ; )
            {
               Console.Write("\r{0:hh:mm:ss}: Sending...", DateTime.Now);
               port.Write(new Byte[] { 0xC0 }, 0, 1);
               while (port.BytesToRead == 0)
                  Thread.Sleep(1);
               Console.Write("received: 0x{0,2}", port.ReadByte().ToString("X"));
               Thread.Sleep(50);
            }
         }
      }
   }
}
