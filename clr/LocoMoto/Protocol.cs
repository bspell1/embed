using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;

namespace LocoMoto
{
   public class Protocol : IDisposable
   {
      public const Int32 BroadcastAddress = 0x00;
      public const Int32 UnknownAddress = 0xFF;
      private const Int32 Signature = 0xC0;
      private enum Command
      {
         Ping = 0,
         SetAddress = 1,
         SetOutput = 2,
         SetPwm = 3,
         StepMotor = 4,
         StopMotor = 5
      };

      private SerialPort port;
      private Int32 address;

      public Protocol (String device, Int32 address)
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

      public void SetOutput (Int32 channel, Boolean value)
      {
         Send(Command.SetOutput, channel, value ? 1 : 0);
      }

      public void MotorStep (Int32 motor, Int32 steps, Int32 delay)
      {
         Send(Command.StepMotor, motor, delay, steps >> 8, steps);
      }

      public void MotorStop (Int32 motor)
      {
         Send(Command.StopMotor, motor);
      }
   }
}
