using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Mono.Unix;
using Mono.Unix.Native;

using NPi;
using NPi.WiiChuk;

namespace Lab
{
   public class Test
   {
      public static Int32 Iterations = 1;
      public static Int32 Threads = 1;
      public static String Param = "";
      public Int32 ThreadID { get; set; }
      public Int32 Iteration { get; set; }
      
      static Test ()
      {
      }

      public Test ()
      {
      }

      public void Run ()
      {
         var locoPath = Directory.GetFiles("/dev", "ttyUSB*").Single();
         using (var reactor = new Reactor())
         using (var loco = new LocoMoto.Driver(locoPath, LocoMoto.Driver.UnknownAddress))
         using (var rx = new Nrf24("/dev/spidev0.0", 17, 25, reactor))
         using (var wii = new WiiChukPair(new Nrf24Receiver(rx, "Wii00")))
         using (var alarm = new Gpio(24, Gpio.Mode.Output))
         {
            alarm.Value = false;
            var lMoto = loco.CreateStepper(0);
            var rMoto = loco.CreateStepper(1);
            lMoto.StepsPerCycle = rMoto.StepsPerCycle = 128;
            lMoto.Reverse = true;
            lMoto.Stop();
            rMoto.Stop();
            rx.Config = new Nrf24.ConfigRegister(rx.Config)
            {
               Mode = Nrf24.Mode.Receive,
               Crc = Nrf24.Crc.TwoByte
            };
            rx.Validate();
            wii.Updated += (l, r) =>
            {
               lMoto.Rpm = (Int32)(l.JoystickY * Stepper.MaxRpm);
               rMoto.Rpm = (Int32)(r.JoystickY * Stepper.MaxRpm);
               alarm.Value = r.ZButton;
            };
            reactor.Start();
            rx.Listen();
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               if (Math.Abs(lMoto.Rpm) < Stepper.MaxRpm / 10)
                  lMoto.Stop();
               else
                  lMoto.Run();
               if (Math.Abs(rMoto.Rpm) < Stepper.MaxRpm / 10)
                  rMoto.Stop();
               else
                  rMoto.Run();
               //Console.Write("\r{0,3} {1,3}          ", lMoto.Rpm, rMoto.Rpm);
               Thread.Sleep(100);
            }
            reactor.Join();
            Console.WriteLine();
         }
      }
   }
}
