using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

using NPi;

namespace LocoMoto
{
   class Program
   {
      static Int32 Main (String[] options)
      {
         Console.WriteLine("LocoMoto");
         if (!ParseOptions(options))
         {
            ReportUsage();
            return 1;
         }
         Execute();
         return 0;
      }

      static Boolean ParseOptions (String[] options)
      {
         // parse options
         try
         {
            new Options.OptionSet()
            {
               { "?|help", v => { throw new ArgumentException(); } }
            }.Parse(options);
         }
         catch { return false; }
         // validate options
         return true;
      }

      static void ReportUsage ()
      {
         Console.WriteLine("   Usage: LocoMoto {options}");
      }

      static void Execute ()
      {
         var controlCfg = new Controller.Config()
         {
            UartPath = Directory.GetFiles("/dev", "ttyAMA*").Single(),
            LocoAddress = LocoMoto.Protocol.BroadcastAddress,
            TrikeStepsPerCycle = 50,
            TrikeMinRpm = 40,
            TrikeMaxRpm = 200,
            TrikeAxleWidth = 18.4,
            TrikeWheelRadius = 2.1
         };
         var inputConfig = new PsxPadInput.Config()
         {
            SpiPath = Directory.GetFiles("/dev", "spidev*.0").Single(),
            CEPin = 17,
            IrqPin = 18,
            Pipe = 0,
            Address = "Psx00"
         };
         using (var reactor = new Reactor())
         using (var control = new Controller(controlCfg))
         using (var input = new PsxPadInput(inputConfig, reactor, control))
         {
            var lastCtrl = DateTime.MinValue;
            input.Changed += () => lastCtrl = DateTime.Now;
            reactor.Start();
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               Console.Write("\rLast controller update: {0:h:mm:ss tt} {1} {2}                  ", lastCtrl, control.Trike.Left.Rpm, control.Trike.Right.Rpm);
               Thread.Sleep(1000);
            }
            reactor.Join();
            control.Trike.Stop();
            Console.WriteLine();
         }
      }
   }
}
