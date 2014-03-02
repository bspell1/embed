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
         var device = Directory.GetFiles("/dev", "ttyAMA*").Single();
         using (var uart = new SerialPort(device, 57600, Parity.None, 8, StopBits.One))
         {
            uart.Open();
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               uart.Write(new Byte[] { 0xAA }, 0, 1);
               Thread.Sleep(10);
            }
         }
         Console.WriteLine();
      }
   }
}
