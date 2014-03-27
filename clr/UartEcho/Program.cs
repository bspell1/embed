using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Threading;

namespace UartEcho
{
   class Program
   {
      static String portName;
      static Int32 baudRate;
      static Parity parityBits;
      static Int32 dataBits;
      static StopBits stopBits;

      static Int32 Main (String[] options)
      {
         Console.WriteLine("UART Echo");
         if (!ParseOptions(options))
         {
            ReportUsage();
            return 1;
         }
         ExecuteEcho();
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
         Console.WriteLine("   Usage: UartEcho {options}");
         Console.WriteLine("      -n|-name {name} port name, default=COM5 or /dev/ttyAMA0");
         Console.WriteLine("      -b|-baud {rate} baud rate, default=57600");
         Console.WriteLine("      -p|-parity {none|even|odd|mark|space} parity bits, default=none");
         Console.WriteLine("      -d|-data {bits} data bits, default=8");
         Console.WriteLine("      -s|-stop {none|one|onepointfive|two} stop bits, default=one");
      }

      static void ExecuteEcho ()
      {
         using (var port = new SerialPort(portName, baudRate, parityBits, dataBits, stopBits))
         {
            Console.Write("   Opening port {0}...", port.PortName);
            port.Open();
            port.DiscardInBuffer();
            Console.WriteLine("done. Now listening. Press escape to exit.");
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               if (port.BytesToRead > 0)
                  Console.Write(port.ReadExisting());
               Thread.Sleep(1);
            }
         }
      }
   }
}
