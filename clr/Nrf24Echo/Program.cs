using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;

using NPi;

namespace Nrf24Echo
{
   class Program
   {
      static String[] addrs;
      static String spiPath;
      static Int32 cePin;
      static Int32 intPin;

      static Int32 Main (String[] options)
      {
         Console.WriteLine("Nrf24 Echo");
         if (ParseOptions(options))
            return Echo();
         ReportUsage();
         return 1;
      }

      static Boolean ParseOptions (String[] options)
      {
         // configure option defaults
         spiPath = Directory.GetFiles("/dev", "spidev*.0").FirstOrDefault();
         cePin = 17;
         intPin = 18;
         // parse options
         try
         {
            var unparsed = new Options.OptionSet()
            {
               { "p|spi-path=", v => spiPath = v },
               { "c|ce-pin=", (Int32 v) => cePin = v },
               { "i|int-pin=", (Int32 v) => intPin = v },
               { "h|?|help", v => { throw new Options.OptionException(); } }
            }.Parse(options);
            addrs = unparsed.ToArray();
         }
         catch { return false; }
         // validate options
         if (addrs.Length == 0)
            return false;
         if (String.IsNullOrWhiteSpace(spiPath))
            return false;
         if (cePin <= 0)
            return false;
         if (intPin <= 0)
            return false;
         return true;
      }

      static void ReportUsage ()
      {
         Console.WriteLine("   Usage: Nrf24Echo {nrf24-addr ...} [options]");
         Console.WriteLine("      {nrf24-addr}         list of NRF24 addresses to listen for (max of 6, 2-6 must have the same 3-digit prefix)");
         Console.WriteLine("      -p|-spi-path {path}  SPI device path for the NRF24 (default: /dev/spidev*.0)");
         Console.WriteLine("      -c|-ce-pin {pin}     GPIO pin for the NRF24 CE pin (default: 17)");
         Console.WriteLine("      -i|-int-pin {pin}    GPIO pin for the NRF24 interrupt pin (default: 18)");
      }
      static void ReportException (Exception e)
      {
         Console.WriteLine();
         Console.WriteLine(
            "   Listen failed: {0}",
            e.Message.Replace("\n", "\n      ")
         );
      }

      static Int32 Echo ()
      {
         try
         {
            using (var reactor = new Reactor())
            using (var nrf24 = new Nrf24(spiPath, cePin, intPin, reactor))
            {
               var updated = new DateTime[addrs.Length];
               var data = new Byte[addrs.Length][];
               var rxEnable = new Nrf24.PipeFlagRegister();
               var autoack = new Nrf24.PipeFlagRegister();
               var dynpld = new Nrf24.PipeFlagRegister();
               nrf24.AddressWidth = addrs[0].Length;
               for (var i = 0; i < addrs.Length; i++)
               {
                  updated[i] = DateTime.MinValue;
                  data[i] = new Byte[Nrf24.MaxPayload];
                  rxEnable[i] = true;
                  autoack[i] = true;
                  dynpld[i] = true;
                  nrf24.SetRXAddress(i, addrs[i]);
               }
               nrf24.Features = new Nrf24.FeatureRegister(nrf24.Features)
               {
                  DynPayload = true
               };
               nrf24.RXEnabled = rxEnable;
               nrf24.AutoAck = autoack;
               nrf24.DynPayload = dynpld;
               nrf24.Config = new Nrf24.ConfigRegister(nrf24.Config)
               {
                  Mode = Nrf24.Mode.Receive
               };
               nrf24.Validate();
               nrf24.RXDataReady += status =>
               {
                  while (status.Interrupts.HasFlag(Nrf24.Interrupt.RXDataReady))
                  {
                     lock (data[status.RXReadyPipe])
                     {
                        updated[status.RXReadyPipe] = DateTime.Now;
                        nrf24.ReceivePacket(data[status.RXReadyPipe]);
                     }
                     status = nrf24.Status;
                  }
               };
               nrf24.Listen();
               reactor.Start();
               Console.WriteLine("   Listening for updates. Press escape to exit.");
               Console.WriteLine();
               var cursorLeft = Console.CursorLeft;
               var cursorTop = Console.CursorTop;
               for (; ; )
               {
                  if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                     break;
                  Console.SetCursorPosition(cursorLeft, cursorTop);
                  for (var i = 0; i < addrs.Length; i++)
                  {
                     var message = String.Empty;
                     lock (data[i])
                        message = String.Format(
                           "   {0,5} ({1,8:h:mm:ss}): {2}",
                           addrs[i],
                           updated[i],
                           BitConverter.ToString(data[i])
                        );
                     Console.WriteLine(message);
                  }
                  Thread.Sleep(20);
               }
            }
            Console.WriteLine();
         }
         catch (Exception e)
         {
            ReportException(e);
            return 1;
         }
         return 0;
      }
   }
}
