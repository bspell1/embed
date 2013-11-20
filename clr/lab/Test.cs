using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Linq;
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
         var spiPath = Directory.GetFiles("/dev", "spidev*.0").Single();
         using (var reactor = new Reactor())
         using (var nrf24 = new Nrf24(spiPath, 17, 18, reactor))
         using (var chuks = new WiiChukPair(new Nrf24Receiver(nrf24, "Wii00", 0)))
         {
            var updated = DateTime.MinValue;
            chuks.Updated += (c1, c2) => updated = DateTime.Now;
            reactor.Start();
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               Console.Write(
                  "\rLx: {0,5:0.00} Ly:{1,5:0.00} Lc:{2} Lz:{3}  |  Rx:{4,5:0.00} Ry:{5,5:0.00} Rc:{6} Rz:{7}", 
                  chuks.Left.JoystickX,
                  chuks.Left.JoystickY,
                  chuks.Left.CButton ? 1 : 0,
                  chuks.Left.ZButton ? 1 : 0,
                  chuks.Right.JoystickX,
                  chuks.Right.JoystickY,
                  chuks.Right.CButton ? 1 : 0,
                  chuks.Right.ZButton ? 1 : 0
               );
               Thread.Sleep(100);
            }
         }
         Console.WriteLine();
      }
   }
}
