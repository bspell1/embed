using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;

using NPi;

namespace QuopEcho
{
   class Program
   {
      static String addr;
      static String spiPath;
      static Int32 cePin;
      static Int32 irqPin;

      static Int32 Main (String[] options)
      {
         Console.WriteLine("Quopter Telemetrics Echo");
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
         irqPin = 0;
         // parse options
         try
         {
            var unparsed = new Options.OptionSet()
            {
               { "spi-path=", v => spiPath = v },
               { "ce-pin=", (Int32 v) => cePin = v },
               { "irq-pin=", (Int32 v) => irqPin = v },
               { "h|?|help", v => { throw new Options.OptionException(); } }
            }.Parse(options);
            addr = unparsed.Single();
         }
         catch { return false; }
         // validate options
         // delegate NRF24 validation to framework
         if (String.IsNullOrWhiteSpace(addr))
            return false;
         if (String.IsNullOrWhiteSpace(spiPath))
            return false;
         if (cePin <= 0)
            return false;
         if (irqPin < 0)
            return false;
         return true;
      }

      static void ReportUsage ()
      {
         Console.WriteLine("   Usage: QuopEcho {quopter-addr} [options]");
         Console.WriteLine("      {quopter-addr}          NRF24 address to listen for");
         Console.WriteLine("      -spi-path {path}        SPI device path for the NRF24 (default: /dev/spidev*.0)");
         Console.WriteLine("      -ce-pin {pin}           GPIO pin for the NRF24 CE pin (default: 17)");
         Console.WriteLine("      -irq-pin {pin}          GPIO pin for the NRF24 interrupt pin (default: none)");
      }
      static void ReportException (Exception e)
      {
         Console.WriteLine();
         Console.WriteLine(
            "   Listen failed: {0}",
            e.ToString().Replace("\n", "\n      ")
         );
      }

      static Int32 Echo ()
      {
         try
         {
            using (var reactor = new Reactor())
            using (var nrf24 = new Nrf24(spiPath, cePin, irqPin, reactor))
            {
               var updated = DateTime.MinValue;
               var data = new Byte[TelemetricsPacket.EncodedSize];
               nrf24.AddressWidth = addr.Length;
               nrf24.RXAddress1 = addr;
               nrf24.RXLength1 = TelemetricsPacket.EncodedSize;
               nrf24.Features = new Nrf24.FeatureRegister(nrf24.Features)
               {
                  DisableAck = true
               };
               nrf24.RXEnabled = new Nrf24.PipeFlagRegister()
               {
                  Pipe1 = true
               };
               nrf24.AutoAck = Nrf24.PipeFlagRegister.None;
               nrf24.Config = new Nrf24.ConfigRegister(nrf24.Config)
               {
                  Mode = Nrf24.Mode.Receive,
                  Crc = Nrf24.Crc.TwoByte,
                  Interrupts = Nrf24.Interrupt.RXDataReady
               };
               nrf24.Validate();
               nrf24.RXDataReady += status =>
               {
                  lock (data)
                  {
                     updated = DateTime.Now;
                     nrf24.ReceivePacket(data);
                  }
               };
               reactor.Start();
               nrf24.Listen();
               var counter = 0;
               var oldCount = 0;
               var started = DateTime.UtcNow;
               Console.WriteLine("   Listening for updates. Press escape to exit.");
               Console.WriteLine();
               for (; ; )
               {
                  if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                     break;
                  var message = "";
                  var packet = default(TelemetricsPacket);
                  lock (data)
                     packet = TelemetricsPacket.Decode(data);
                  if (counter == 0)
                  {
                     oldCount = packet.Counter - 1;
                     started = DateTime.UtcNow;
                  }
                  var countDiff = packet.Counter >= oldCount ? packet.Counter - oldCount : Byte.MaxValue - oldCount + packet.Counter;
                  var newCounter = counter + countDiff;
                  if (newCounter >= counter)
                     counter = newCounter;
                  else
                  {
                     started = DateTime.UtcNow;
                     counter = 0;
                  }
                  oldCount = packet.Counter;
                  var cps = (Double)counter / (DateTime.UtcNow - started).TotalSeconds;
                  message = String.Format(
                     "\r   {0,-8:h:mm:ss}: Rs={1,-4} Ps={2,-4} Ys={3,-4} T={4,-3} Ri={5,-4} Pi={6,-4} Yi={7,-4} Rbo={8,-4} Rst={9,-4} Rpt={10,-4} Rsb={11,-4} Rp={12,-4} Rr={13,-4} C={14,-4} CL={15,-6:0.0ms}      ",
                     updated,
                     packet.RollAngle,
                     packet.PitchAngle,
                     packet.YawRate,
                     packet.ThrustInput,
                     packet.RollInput,
                     packet.PitchInput,
                     packet.YawInput,
                     packet.BowRotor,
                     packet.SternRotor,
                     packet.PortRotor,
                     packet.StarboardRotor,
                     packet.BowRotor - packet.SternRotor,
                     packet.PortRotor - packet.StarboardRotor,
                     packet.Counter,
                     1000.0 / cps
                  );
                  Console.Write(message);
                  Thread.Sleep(100);
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
