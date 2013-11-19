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
         using (var mpu = new Mpu6050("/dev/i2c-1"))
         {
            mpu.Wake();
            mpu.PowerConfig = new Mpu6050.PowerConfigRegister(mpu.PowerConfig)
            {
               ClockSource = Mpu6050.ClockSource.PllGyroX
            };
            mpu.GeneralConfig = new Mpu6050.GeneralConfigRegister(mpu.GeneralConfig)
            {
               //Filter = Mpu6050.LowPassFilter._5Hz
            };
            mpu.GyroConfig = new Mpu6050.GyroConfigRegister(mpu.GyroConfig)
            {
               //Scale = Mpu6050.GyroScale._1000DegPerSec
            };
            Console.WriteLine(mpu.GeneralConfig);
            var ticks2 = 20.0f;
            Single filterX = 0.0f;
            Single filterY = 0.0f;
            Single yawZ = 0.0f;
            var l = new Object();
            var done = false;
            var thread = new Thread(
               () =>
               {
                  var watch = new Stopwatch();
                  var dt = 0.001f;
                  while (!done)
                  {
                     watch.Reset();
                     watch.Start();
                     //Thread.Sleep(10);
                     var sample = mpu.Sensors;
                     Single accelX = sample.Accel.X.Value * 2.0f;
                     Single accelY = sample.Accel.Y.Value * 2.0f;
                     Single accelZ = sample.Accel.Z.Value * 2.0f;
                     Single angleX = accelX * (Single)Math.PI / 2.0f; // Native.Atan2(accelX, (Single)Math.Sqrt(accelY * accelY + accelZ * accelZ));
                     Single angleY = accelY * (Single)Math.PI / 2.0f; // Native.Atan2(accelY, (Single)Math.Sqrt(accelX * accelX + accelZ * accelZ));
                     Single rateX = sample.Gyro.X.Value * 250.0f / 180.0f * (Single)Math.PI;
                     Single rateY = sample.Gyro.Y.Value * 250.0f / 180.0f * (Single)Math.PI;
                     Single rateZ = sample.Gyro.Z.Value * 250.0f / 180.0f * (Single)Math.PI;
                     watch.Stop();
                     lock (l)
                     {
                        filterX = ComplementaryFilter(filterX, angleX, rateX, dt);
                        filterY = ComplementaryFilter(filterY, angleY, rateY, dt);
                        yawZ = rateZ;
                        ticks2 = dt * 1000;
                     }
                     dt = (Single)(watch.Elapsed.TotalMilliseconds / 1000);
                  }
               }
            );
            thread.Start();
            for (; ; )
            {
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
               Single fX;
               Single fY;
               Single yZ;
               Single t2;
               lock (l)
               {
                  fX = filterX;
                  fY = filterY;
                  yZ = yawZ;
                  t2 = ticks2;
               }
               Console.Write("\rFx: {0,5:0.00} Fy:{1,5:0.00} Yz:{2,5:0.00} Dt:{3,4:0.00}  ", fX, fY, yZ, t2);
               Thread.Sleep(10);
            }
            done = true;
            thread.Join();
         }
      }

      private Single ComplementaryFilter (Single filterAngle, Single accelAngle, Single gyroRate, Single dt)
      {
         //Single dt = 0.030f;
         //Single tc = 0.750f;
         //Single bias = tc / (tc + dt);
         Single bias = 0.95f;
         Single result = bias * (filterAngle + gyroRate * dt) + (1.0f - bias) * accelAngle;
         return (Single)Math.Min(Math.Max(result, -Math.PI / 2), Math.PI / 2);
      }
   }
}
