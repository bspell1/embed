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
         using (var loco = new LocoMoto.Driver("/dev/ttyUSB0", 0xFF))
         {
            var motor0 = loco.CreateStepper(0);
            var motor1 = loco.CreateStepper(1);
            motor0.StepsPerCycle = motor1.StepsPerCycle = 128;
            for (; ; )
            {
               motor0.Run();
               motor1.RunReverse();
               Thread.Sleep(2000);
               motor0.Stop();
               motor1.Stop();
               Thread.Sleep(1000);
            }
         }
#if false
         using (var i2c = new I2CDevice("/dev/i2c-1"))
         {
            i2c.SlaveAddress = 0x3E;
            i2c.Write(new Byte[] { 0x0F, 0 });
            i2c.Write(new Byte[] { 0x11, 0xFF });
         }
#endif

#if false
         var tx = Encoding.ASCII.GetBytes("0123456789ABCDEF");
         var rx = new Byte[tx.Length];
         using (var spi = new SpiDevice("/dev/spidev0.0"))
         {
            spi.ReadWrite(tx, rx);
            Console.Write(Encoding.ASCII.GetString(rx));
         }
#endif

#if false
         Thread.CurrentThread.Priority = ThreadPriority.Highest;
         using (var motor = new StepperMotor(512, 22, 23, 24, 25))
         {
            // TODO: deal with linux signals
            motor.Rpm = 40;
            motor.Rotate(8 * Math.PI);
         }
#endif

#if false
         for (; ; )
         {
            try
            {
               Console.Write("Connecting to the camera...");
               using (var camera = new VC0706Camera("/dev/ttyAMA0"))
               {
                  Console.WriteLine("done.");
                  Console.WriteLine("Camera version: {0}", camera.Version);

                  var frameBuffer = new Byte[65536];
                  var basePath = "/mnt/liono-temp";
                  var camPath = Path.Combine(basePath, "camera0.jpg");
                  var frameLen = camera.BeginCaptureImage();
                  for (; ; )
                  {
                     camera.EndCaptureImage(frameBuffer, frameLen);
                     Console.WriteLine("{0:mm/dd hh:mm:ss}: Got image,  {1} bytes", DateTime.Now, frameLen);
                     var nextFrameLen = camera.BeginCaptureImage();
                     var tmpPath = Path.Combine(basePath, Path.GetRandomFileName());
                     if (File.Exists(camPath))
                     {
                        File.Move(camPath, tmpPath);
                        File.Delete(tmpPath);
                        tmpPath = Path.Combine(basePath, Path.GetRandomFileName());
                     }
                     try
                     {
                        using (var f = File.Create(tmpPath))
                           f.Write(frameBuffer, 0, frameLen);
                        File.Move(tmpPath, camPath);
                     }
                     catch
                     {
                        File.Delete(tmpPath);
                        throw;
                     }
                     frameLen = nextFrameLen;
                  }
               }
            }
            catch (TimeoutException) { }
         }
#endif
      }
   }
}
