using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Threading;

namespace Lab
{
   class Program
   {
      static String portName;
      static Int32 baudRate;
      static Parity parityBits;
      static Int32 dataBits;
      static StopBits stopBits;
      static Int32 message;

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
         // apply option defaults
         var portNames = SerialPort.GetPortNames();
         portName = 
            portNames.FirstOrDefault(p => p.StartsWith("/dev/ttyUSB")) ??
            portNames.FirstOrDefault(p => p.StartsWith("/dev/ttyAMA")) ??
            "COM5";
         baudRate = 57600;
         parityBits = Parity.None;
         dataBits = 8;
         stopBits = StopBits.One;
         message = 0xC0;
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
         Console.WriteLine("      -n|-name {name} port name, default=COM5 or /dev/ttyAMA0");
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
            port.DiscardInBuffer();
            Console.WriteLine("done. Press escape to exit.");
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               Console.Write("\r   {0:h:mm:ss tt}: Sending 0x{1}... ", DateTime.Now, message.ToString("X"));
               port.Write(new[] { (Byte)message }, 0, 1);
               while (port.BytesToRead == 0)
                  Thread.Sleep(1);
               Console.Write("Received: 0x{0}", port.ReadByte().ToString("X"));
               Thread.Sleep(1000);
            }
            Console.WriteLine();
         }
      }
   }
}
