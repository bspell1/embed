using System;
using System.Collections.Generic;
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
#if true
         using (var mpu = new Mpu6050("/dev/i2c-1"))
         {
            mpu.Wake();
            mpu.PowerConfig = new Mpu6050.PowerConfigRegister(mpu.PowerConfig)
            {
               ClockSource = Mpu6050.ClockSource.PllGyroX
            };
            mpu.GeneralConfig = new Mpu6050.GeneralConfigRegister(mpu.GeneralConfig)
            {
               Filter = Mpu6050.LowPassFilter._5Hz
            };
            Console.WriteLine(mpu.GeneralConfig);
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               Console.Write(
                  "\rTemperature: {0} Gx: {1} Gy: {2} Gz: {3} Ax: {4} Ay: {5} Az: {6}      ", 
                  mpu.Temperature, 
                  mpu.GyroX, 
                  mpu.GyroY, 
                  mpu.GyroZ,
                  mpu.AccelX, 
                  mpu.AccelY, 
                  mpu.AccelZ
               );
               Thread.Sleep(10);
            }
            Console.WriteLine(mpu.Temperature);
         }
#else
         using (var i2c = new I2CDevice("/dev/i2c-1"))
         {
            i2c.SlaveAddress = 0x68;
            var buffer = new Byte[3];
            buffer[0] = 0x6B;
            i2c.ReadWrite(buffer, 0, 1, buffer, 1, 2);
            //buffer[0] |= (1 << 7);
            unchecked
            {
               buffer[1] &= (Byte)~(1 << 6);
               buffer[1] &= (Byte)~(1 << 3);
            }
            i2c.Write(buffer, 3);
            buffer[0] = 0x41;
            i2c.ReadWrite(buffer, 0, 1, buffer, 1, 2);
            var tempraw = (Int16)((UInt16)buffer[1] << 8 | buffer[2]);
            var tempcel = (Double)tempraw / 340 + 35;
            var tempfah = tempcel * 9 / 5 + 32;
            Console.WriteLine("{0}", BitConverter.ToString(buffer));
            Console.WriteLine(tempraw);
            Console.WriteLine(tempcel);
            Console.WriteLine(tempfah);
         }
#endif
      }
   }
}
