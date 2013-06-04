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
#if false
         using (var chuk = new WiiChuk("/dev/i2c-1", 25, false))
         {
            for (; ; )
            {
               chuk.Read();
               Console.WriteLine(new String('-', 78));
               Console.WriteLine("Time:           {0:h:mm:ss tt}", DateTime.Now);
               Console.WriteLine("JoystickX:      {0}", chuk.JoystickX);
               Console.WriteLine("JoystickY:      {0}", chuk.JoystickY);
               Console.WriteLine("AccelerometerX: {0}", chuk.AccelerometerX);
               Console.WriteLine("AccelerometerY: {0}", chuk.AccelerometerY);
               Console.WriteLine("AccelerometerZ: {0}", chuk.AccelerometerZ);
               Console.WriteLine("CButton:        {0}", chuk.CButton);
               Console.WriteLine("ZButton:        {0}", chuk.ZButton);
               Console.WriteLine("LastCButton:    {0}", chuk.LastCButton);
               Console.WriteLine("LastZButton:    {0}", chuk.LastZButton);
               Thread.Sleep(1000);
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
            }
         }
#endif

#if true
         using (var wii = new Nrf24("/dev/spidev0.0", 17))
         {
            wii.RXAddress0 = "Wii00";
            wii.RXLength0 = 12;
            // start up the transmitter/receiver
            wii.Config = new Nrf24.ConfigRegister(wii.Config)
            {
               Crc = Nrf24.Crc.None
            };
            wii.RFChannel = 60;
            wii.RFConfig = new Nrf24.RFConfigRegister(wii.RFConfig)
            {
               BitRate = Nrf24.BitRate.OneMbps
            };
            wii.Config = new Nrf24.ConfigRegister(wii.Config)
               { Mode = Nrf24.Mode.Receive };
            Console.WriteLine(wii.DumpRegisters());
            for (; ; )
            {
               wii.BeginReceivePacket();
               try
               {
                  Console.Write("{0:h:mm:ss tt}: Receiving (carrier = {1})...", DateTime.Now, wii.CarrierDetect);
                  var result = wii.EndReceivePacket(12);
                  Console.WriteLine("done ({0}).", BitConverter.ToString(result));
               }
               catch (TimeoutException)
               {
                  Console.WriteLine("timeout.");
               }
               wii.ClearInterrupts();
               if (Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape)
                  break;
            }
         }
#endif

#if false
         var path = Directory.GetFiles("/dev", "ttyUSB*").Single();
         Console.WriteLine("Connecting to locomoto on {0}.", path);
         using (var loco = new LocoMoto.Driver(path, LocoMoto.Driver.UnknownAddress))
         {
            var motor0 = loco.CreateStepper(0);
            var motor1 = loco.CreateStepper(1);
            motor0.StepsPerCycle = motor1.StepsPerCycle = 128;
            motor0.Rpm = motor1.Rpm = Stepper.MinRpm;
            motor1.Reverse = true;
            motor0.Stop();
            motor1.Stop();
            var rpm0 = 0;
            var rpm1 = 0;
            var rpmi = 10;
            for (; ; )
            {
               var key = Console.ReadKey().Key;
               switch (key)
               {
                  case ConsoleKey.W:
                     rpm0 = Math.Min(Math.Max(rpm0 + rpmi, -Stepper.MaxRpm), Stepper.MaxRpm);
                     motor0.Rpm = Math.Min(Math.Max(Math.Abs(rpm0), Stepper.MinRpm), Stepper.MaxRpm);
                     break;
                  case ConsoleKey.S:
                     rpm0 = Math.Min(Math.Max(rpm0 - rpmi, -Stepper.MaxRpm), Stepper.MaxRpm);
                     motor0.Rpm = Math.Min(Math.Max(Math.Abs(rpm0), Stepper.MinRpm), Stepper.MaxRpm);
                     break;
                  case ConsoleKey.O:
                     rpm1 = Math.Min(Math.Max(rpm1 + rpmi, -Stepper.MaxRpm), Stepper.MaxRpm);
                     motor1.Rpm = Math.Min(Math.Max(Math.Abs(rpm1), Stepper.MinRpm), Stepper.MaxRpm);
                     break;
                  case ConsoleKey.L:
                     rpm1 = Math.Min(Math.Max(rpm1 - rpmi, -Stepper.MaxRpm), Stepper.MaxRpm);
                     motor1.Rpm = Math.Min(Math.Max(Math.Abs(rpm1), Stepper.MinRpm), Stepper.MaxRpm);
                     break;
                  case ConsoleKey.Spacebar:
                     motor0.Stop();
                     motor1.Stop();
                     rpm0 = rpm1 = 0;
                     break;
                  default:
                     break;
               }
               if (key == ConsoleKey.W || key == ConsoleKey.S)
               {
                  if (rpm0 == 0)
                     motor0.Stop();
                  else if (rpm0 > 0)
                     motor0.Run();
                  else
                     motor0.RunReverse();
               }
               if (key == ConsoleKey.O || key == ConsoleKey.L)
               {
                  if (rpm1 == 0)
                     motor1.Stop();
                  else if (rpm1 > 0)
                     motor1.Run();
                  else
                     motor1.RunReverse();
               }
            }
         }
#endif

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
