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
      static Int32 irqPin;
      static Int32 rxLength;
      static Int32 crcLength;
      static Boolean autoAck;
      static Int32 dataRate;
      static Int32 rfChannel;

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
         irqPin = 0;
         rxLength = 0;
         crcLength = 8;
         autoAck = true;
         dataRate = 2;
         rfChannel = 2;
         // parse options
         try
         {
            var unparsed = new Options.OptionSet()
            {
               { "spi-path=", v => spiPath = v },
               { "ce-pin=", (Int32 v) => cePin = v },
               { "irq-pin=", (Int32 v) => irqPin = v },
               { "rx-len=", (Int32 v) => rxLength = v },
               { "crc-len=", (Int32 v) => crcLength = v },
               { "no-ack", v => autoAck = v == null },
               { "data-rate=", (Int32 v) => dataRate = v },
               { "rf-channel=", (Int32 v) => rfChannel = v },
               { "h|?|help", v => { throw new Options.OptionException(); } }
            }.Parse(options);
            addrs = unparsed.ToArray();
         }
         catch { return false; }
         // validate options
         // delegate NRF24 validation to framework
         if (addrs.Length == 0)
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
         Console.WriteLine("   Usage: Nrf24Echo {nrf24-addr ...} [options]");
         Console.WriteLine("      {nrf24-addr}            list of NRF24 addresses to listen for (max of 6, 2-6 must have the same 3-digit prefix)");
         Console.WriteLine("      -spi-path {path}        SPI device path for the NRF24 (default: /dev/spidev*.0)");
         Console.WriteLine("      -ce-pin {pin}           GPIO pin for the NRF24 CE pin (default: 17)");
         Console.WriteLine("      -irq-pin {pin}          GPIO pin for the NRF24 interrupt pin (default: none)");
         Console.WriteLine("      -rx-len {length}        receive packet length (default: dynamic)");
         Console.WriteLine("      -crc-len {length}       CRC length, in bits (0, 8, or 16, default: 8)");
         Console.WriteLine("      -data-rate {rate}       air data rate, in Mbps (1 or 2, default: 2)");
         Console.WriteLine("      -no-ack                 disable auto acknowledge");
         Console.WriteLine("      -rf-channel {channel}   radio frequency channel, in 1MHz units (1-126 default: 2)");
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
               var updated = new DateTime[addrs.Length];
               var data = new Byte[addrs.Length][];
               var rxEnable = new Nrf24.PipeFlagRegister();
               var enAck = new Nrf24.PipeFlagRegister();
               var dynpld = new Nrf24.PipeFlagRegister();
               nrf24.AddressWidth = addrs[0].Length;
               for (var i = 0; i < addrs.Length; i++)
               {
                  updated[i] = DateTime.MinValue;
                  data[i] = new Byte[rxLength != 0 ? rxLength : Nrf24.MaxPayload];
                  rxEnable[i] = true;
                  enAck[i] = autoAck;
                  dynpld[i] = rxLength == 0;
                  nrf24.SetRXAddress(i, addrs[i]);
                  nrf24.SetRXLength(i, rxLength);
                  addrs[i] = nrf24.GetRXAddress(i);
               }
               nrf24.Features = new Nrf24.FeatureRegister(nrf24.Features)
               {
                  DisableAck = true,
                  DynPayload = rxLength == 0
               };
               nrf24.RXEnabled = rxEnable;
               nrf24.AutoAck = enAck;
               nrf24.DynPayload = dynpld;
               nrf24.RFChannel = rfChannel;
               nrf24.RFConfig = new Nrf24.RFConfigRegister(nrf24.RFConfig)
               {
                  BitRate = dataRate == 1 ? 
                            Nrf24.BitRate.OneMbps : 
                            Nrf24.BitRate.TwoMbps
               };
               nrf24.Config = new Nrf24.ConfigRegister(nrf24.Config)
               {
                  Mode = Nrf24.Mode.Receive,
                  Crc = crcLength == 16 ? 
                        Nrf24.Crc.TwoByte : 
                        crcLength == 8 ? 
                        Nrf24.Crc.OneByte :
                        Nrf24.Crc.None,
                  Interrupts = Nrf24.Interrupt.RXDataReady
               };
               nrf24.Validate();
               nrf24.RXDataReady += status =>
               {
                  lock (data[status.RXReadyPipe])
                  {
                     updated[status.RXReadyPipe] = DateTime.Now;
                     var pktRXLength = rxLength != 0 ? rxLength : nrf24.GetRXDynamicLength();
                     nrf24.ReceivePacket(data[status.RXReadyPipe], pktRXLength);
                     for (var i = pktRXLength; i < rxLength; i++)
                        data[status.RXReadyPipe][i] = 0;
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
                           "   {0,5}: {1,-8:h:mm:ss} - {2}",
                           addrs[i],
                           updated[i],
                           BitConverter.ToString(data[i]).Replace('-', ' ')
                        );
                     Console.WriteLine(message);
                  }
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
