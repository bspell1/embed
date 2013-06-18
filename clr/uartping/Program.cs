using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Threading;

namespace Lab
{
   class Program
   {
      static String portName = "COM5";
      static Int32 baudRate = 57600;
      static Parity parityBits = Parity.None;
      static Int32 dataBits = 8;
      static StopBits stopBits = StopBits.One;
      static Int32 message = 0xC0;

      static Int32 Main (String[] options)
      {
         Console.WriteLine("UART Ping");
         if (!ParseOptions(options))
         {
            ReportUsage();
            return 1;
         }
         ExecutePing();
         return 0;
      }

      static Boolean ParseOptions (String[] options)
      {
         // parse options
         try
         {
            new Options.OptionSet()
            {
               { "n|name=", v => portName = v },
               { "b|baud=", (Int32 v) => baudRate = v },
               { "p|parity=", v => parityBits = (Parity)Enum.Parse(typeof(Parity), v, true) },
               { "d|data=", (Int32 v) => dataBits = v },
               { "s|stop=", v => stopBits = (StopBits)Enum.Parse(typeof(StopBits), v, true) },
               { "?|help", v => { throw new ArgumentException(); } }
            }.Parse(options);
         }
         catch { return false; }
         return true;
      }

      static void ReportUsage ()
      {
         Console.WriteLine("   Usage: UartPing {options}");
         Console.WriteLine("      -n|-name {name} port name, ex: /dev/ttyAMA0, default=COM5");
         Console.WriteLine("      -b|-baud {rate} baud rate, default=57600");
         Console.WriteLine("      -p|-parity {none|even|odd|mark|space} parity bits, default=none");
         Console.WriteLine("      -d|-data {bits} data bits, default=8");
         Console.WriteLine("      -s|-stop {none|one|onepointfive|two} stop bits, default=one");
         Console.WriteLine("      -m|-message {msg} ping message, default=0xC0");
      }

      static void ExecutePing ()
      {
         using (var port = new SerialPort(portName, baudRate, parityBits, dataBits, stopBits))
         {
            Console.Write("   Opening port {0}...", port.PortName);
            port.Open();
            port.ReadExisting();
            Console.WriteLine("done.");
            for (; ; )
            {
               Console.Write("\r   {0:hh:mm:ss}: Sending 0x{1}...", DateTime.Now, message.ToString("X"));
               port.Write(new[] { (Byte)message }, 0, 1);
               while (port.BytesToRead == 0)
                  Thread.Sleep(1);
               Console.Write("received: 0x{0}", port.ReadByte().ToString("X"));
               Thread.Sleep(1000);
            }
         }
      }
   }
}
