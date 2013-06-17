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
         using (var reactor = new Reactor())
         using (var rx = new Nrf24("/dev/spidev0.0", 17, 18, reactor))
         using (var wii = new WiiChukPair(new Nrf24Receiver(rx, "Wii00")))
         {
            rx.Config = new Nrf24.ConfigRegister(rx.Config)
            {
               Mode = Nrf24.Mode.Receive,
               Crc = Nrf24.Crc.TwoByte
            };
            rx.Validate();
            reactor.Start();
            rx.Listen();
            Console.WriteLine("Listening...");
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               Thread.Sleep(500);
               Console.Write(
                  "\r{0:hh:mm:ss}: Jx={1,6} Jy={2,6} Ax={3,6} Ay={4,6} Az={5,6}             Jx={6,6} Jy={7,6} Ax={8,6} Ay={9,6} Az={10,6}",
                  DateTime.Now,
                  wii.Left.JoystickX,
                  wii.Left.JoystickY,
                  wii.Left.AcceleroX,
                  wii.Left.AcceleroY,
                  wii.Left.AcceleroZ,
                  wii.Right.JoystickX,
                  wii.Right.JoystickY,
                  wii.Right.AcceleroX,
                  wii.Right.AcceleroY,
                  wii.Right.AcceleroZ
               );
            }
            reactor.Join();
            Console.WriteLine();
         }
#if false
         var locoPath = Directory.GetFiles("/dev", "ttyAMA*").Single();
         using (var reactor = new Reactor())
         using (var loco = new LocoMoto.Driver(locoPath, LocoMoto.Driver.UnknownAddress))
         using (var rx = new Nrf24("/dev/spidev0.0", 17, 18, reactor))
         using (var wii = new WiiChukPair(new Nrf24Receiver(rx, "Wii00")))
         {
            var lMoto = loco.CreateStepper(0);
            var rMoto = loco.CreateStepper(1);
            lMoto.StepsPerCycle = rMoto.StepsPerCycle = 128;
            lMoto.Reverse = true;
            rx.Config = new Nrf24.ConfigRegister(rx.Config)
            {
               Mode = Nrf24.Mode.Receive,
               Crc = Nrf24.Crc.TwoByte
            };
            rx.Validate();
            rx.Listen();
            wii.Updated += (l, r) =>
            {
               // TODO: call Run() here for lock-step responsiveness
               //       this will require modifying the stepper driver
               //       to more smoothly handle transitions (needs testing)
               lMoto.Rpm = (Int32)(l.JoystickY * Stepper.MaxRpm);
               rMoto.Rpm = (Int32)(r.JoystickY * Stepper.MaxRpm);
            };
            reactor.Start();
            var oldLRpm = 0;
            var oldRRpm = 0;
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               if (lMoto.Rpm != oldLRpm)
               {
                  lMoto.Run();
                  oldLRpm = lMoto.Rpm;
               }
               if (rMoto.Rpm != oldRRpm)
               {
                  rMoto.Run();
                  oldRRpm = rMoto.Rpm;
               }
               Console.Write("\r{0,3} {1,3}          ", lMoto.Rpm, rMoto.Rpm);
               Thread.Sleep(50);
            }
            reactor.Join();
            Console.WriteLine();
         }
#endif
      }
   }
}
