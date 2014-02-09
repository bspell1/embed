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
         using (var spi = new SpiDevice(spiPath))
         {
            spi.LsbFirst = true;
            spi.ClockSpeed = 100000;
            spi.Mode = SpiMode.CPHA | SpiMode.CPOL;
            // enter config mode
            {
               var buffer = new Byte[5];
               buffer[0] = 0x01;
               buffer[1] = 0x43;
               buffer[2] = 0x00;
               buffer[3] = 0x01;
               spi.Write(buffer);
            }
            // switch to analog mode
            {
               var buffer = new Byte[6];
               buffer[0] = 0x01;
               buffer[1] = 0x44;
               buffer[2] = 0x00;
               buffer[3] = 0x01;
               buffer[4] = 0x00;
               spi.Write(buffer);
            }
            // exit config mode
            {
               var buffer = new Byte[5];
               buffer[0] = 0x01;
               buffer[1] = 0x43;
               buffer[2] = 0x00;
               buffer[3] = 0x00;
               spi.Write(buffer);
            }
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               var buffer = new Byte[21];
               buffer[0] = 0x01;
               buffer[1] = 0x42;
               spi.ReadWrite(buffer);
               Console.Write(
                  "\r{0}",
                  BitConverter.ToString(buffer)
               );
               Thread.Sleep(10);
            }
         }
         Console.WriteLine();
      }
   }
}
