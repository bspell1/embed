﻿using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;

namespace LocoMoto
{
   public class Driver : IDisposable
   {
      public const Int32 BroadcastAddress = 0x00;
      public const Int32 UnknownAddress = 0xFF;
      private const Int32 Signature = 0xC0;
      private enum Command
      {
         Ping = 0,
         SetAddress = 1,
         SetPwm = 2,
         StepMotor = 3,
         StopMotor = 4
      };

      private SerialPort port;
      private Int32 address;

      public Driver (String device, Int32 address)
      {
         this.port = new SerialPort(device, 57600, Parity.None, 8, StopBits.One);
         this.port.Open();
         this.address = address;
      }

      public void Dispose ()
      {
         if (this.port != null)
            this.port.Dispose();
         this.port = null;
      }

      private void Send (Command command, params Int32[] data)
      {
         while (port.BytesToRead > 0)
            Console.Write("\r{0}", port.ReadByte().ToString("X"));
         var message = new Byte[data.Length + 3];
         message[0] = (Byte)Signature;
         message[1] = (Byte)this.address;
         message[2] = (Byte)command;
         for (var i = 0; i < data.Length; i++)
            message[i + 3] = (Byte)data[i];
         lock (this)
            this.port.Write(message, 0, message.Length);
      }

      public Int32 Ping (Int32 value)
      {
         Send(Command.Ping, value);
         return this.port.ReadByte();
      }

      public NPi.Stepper CreateStepper (Int32 motorNumber)
      {
         return new NPi.Stepper(new StepperDriver(this, motorNumber));
      }


      private class StepperDriver : NPi.IStepperDriver
      {
         private Driver driver;
         private Int32 motor;

         public StepperDriver (Driver driver, Int32 motor)
         {
            this.driver = driver;
            this.motor = motor;
         }

         #region IStepperDriver Implementation
         public Int32 StepsPerCycle
         {
            get; set;
         }
         public void Stop ()
         {
            this.driver.Send(Command.StopMotor, this.motor);
         }
         public void Step (Int32 steps, Int32 rpm)
         {
            if (steps == Int32.MaxValue)
               steps = Int16.MaxValue;
            else if (steps >= Int16.MaxValue)
               throw new ArgumentOutOfRangeException("steps");
            if (steps == Int32.MinValue)
               steps = Int16.MinValue;
            else if (steps <= Int16.MinValue)
               throw new ArgumentOutOfRangeException("steps");
            if (rpm < 0)
            {
               steps *= -1;
               rpm *= -1;
            }
            var delay = 600000 / (rpm * this.StepsPerCycle * 4);
            this.driver.Send(Command.StepMotor, this.motor, delay, steps >> 8, steps);
         }
         #endregion
      }
   }
}
