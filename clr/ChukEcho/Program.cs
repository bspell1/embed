using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;

using NPi;
using NPi.WiiChuk;

namespace ChukEcho
{
   class Program
   {
      static String wiiName;
      static String nrf24Path;
      static Int32 nrf24CePin;
      static Int32 nrf24IrqPin;
      static Int32 nrf24Pipe;

      static Int32 Main (String[] options)
      {
         Console.WriteLine("WiiChuk Echo");
         if (ParseOptions(options))
            return Echo();
         ReportUsage();
         return 1;
      }

      static Boolean ParseOptions (String[] options)
      {
         // configure option defaults
         wiiName = "Wii00";
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
               wiiName = unparsed.Single();
         }
         catch { return false; }
         // validate options
         if (String.IsNullOrWhiteSpace(wiiName))
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
         Console.WriteLine("   Usage: ChukEcho [wii-name] [options]");
         Console.WriteLine("      [wii-name]           NRF24 Wii address (default: Wii00)");
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
            using (var chuks = new WiiChukPair(new Nrf24Receiver(nrf24, wiiName, nrf24Pipe)))
            {
               var updated = DateTime.MinValue;
               chuks.Updated += (c1, c2) => updated = DateTime.Now;
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
                  Console.Write(
                     "\r   {0,8:h:mm:ss}: Lx={1,5:0.00} Ly={2,5:0.00} Lc={3} Lz={4}  |  Rx={5,5:0.00} Ry={6,5:0.00} Rc={7} Rz={8}",
                     updated,
                     chuks.Left.JoystickX,
                     chuks.Left.JoystickY,
                     chuks.Left.CButton ? 1 : 0,
                     chuks.Left.ZButton ? 1 : 0,
                     chuks.Right.JoystickX,
                     chuks.Right.JoystickY,
                     chuks.Right.CButton ? 1 : 0,
                     chuks.Right.ZButton ? 1 : 0
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
