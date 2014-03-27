using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;

using NPi;
using NPi.Psx;

namespace PsxEcho
{
   class Program
   {
      static String psxName;
      static String nrf24Path;
      static Int32 nrf24CePin;
      static Int32 nrf24IrqPin;
      static Int32 nrf24Pipe;

      static Int32 Main (String[] options)
      {
         Console.WriteLine("PSX Pad Echo");
         if (ParseOptions(options))
            return Echo();
         ReportUsage();
         return 1;
      }

      static Boolean ParseOptions (String[] options)
      {
         // configure option defaults
         psxName = "Psx00";
         nrf24Path = Directory.GetFiles("/dev", "spidev*.0").FirstOrDefault();
         nrf24CePin = 17;
         nrf24IrqPin = 0;
         nrf24Pipe = 0;
         // parse options
         try
         {
            var unparsed = new Options.OptionSet()
            {
               { "p|nrf24-path=", v => nrf24Path = v },
               { "c|nrf24-ce=", (Int32 v) => nrf24CePin = v },
               { "i|nrf24-irq=", (Int32 v) => nrf24IrqPin = v },
               { "e|nrf24-pipe=", (Int32 v) => nrf24Pipe = v },
               { "h|?|help", v => { throw new Options.OptionException(); } }
            }.Parse(options);
            if (unparsed.Any())
               psxName = unparsed.Single();
         }
         catch { return false; }
         // validate options
         if (String.IsNullOrWhiteSpace(psxName))
            return false;
         if (String.IsNullOrWhiteSpace(nrf24Path))
            return false;
         if (nrf24CePin <= 0)
            return false;
         if (nrf24IrqPin < 0)
            return false;
         if (nrf24Pipe < 0 || nrf24Pipe > 5)
            return false;
         return true;
      }

      static void ReportUsage ()
      {
         Console.WriteLine("   Usage: PsxEcho [psx-name] [options]");
         Console.WriteLine("      [psx-name]              NRF24 PSX pad address (default: Psx00)");
         Console.WriteLine("      -p|-nrf24-path {path}   SPI device path for the NRF24 (default: /dev/spidev*.0)");
         Console.WriteLine("      -c|-nrf24-ce {pin}      GPIO pin for the NRF24 CE pin (default: 17)");
         Console.WriteLine("      -i|-nrf24-irq {pin}     GPIO pin for the NRF24 interrupt pin (default: none/poll)");
         Console.WriteLine("      -e|-nrf24-pipe {pipe}   NRF24 pipe number to use (0-5, default: 0)");
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
            using (var nrf24 = new Nrf24(nrf24Path, nrf24CePin, nrf24IrqPin, reactor))
            {
               var psx = new Nrf24Receiver(nrf24, psxName, nrf24Pipe);
               var updated = DateTime.MinValue;
               var psxState = default(PsxPadState);
               psx.Received += (state) =>
               {
                  lock (psx)
                  {
                     updated = DateTime.Now;
                     psxState = state;
                  }
               };
               nrf24.Config = new Nrf24.ConfigRegister(nrf24.Config)
               {
                  Mode = Nrf24.Mode.Receive,
                  Crc = Nrf24.Crc.TwoByte
               };
               nrf24.Listen();
               reactor.Start();
               Console.WriteLine("   Listening for updates. Press escape to exit.");
               Console.WriteLine();
               for (; ; )
               {
                  if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                     break;
                  lock (psx)
                     Console.Write(
                        "\r   {0,-8:h:mm:ss}: {1}",
                        updated,
                        psxState
                     );
                  Thread.Sleep(20);
               }
               reactor.Join();
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
