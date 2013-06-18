using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using Tpl = System.Threading.Tasks;

namespace NPi.WiiChuk
{
   public class I2cReceiver : IDisposable
   {
      private I2CDevice device;
      private Gpio csPin;
      private Byte[] buffer;
      private Tpl.Task task;

      public I2cReceiver (String i2cPath, Reactor reactor, Int32 csPin = -1)
      {
         try
         {
            this.device = new I2CDevice(i2cPath);
            this.device.SlaveAddress = 0x52;
            if (csPin == -1)
               this.buffer = new Byte[WiiChukState.EncodedSize];
            else
            {
               this.csPin = new Gpio(csPin, Gpio.Mode.Output);
               this.buffer = new Byte[2 * WiiChukState.EncodedSize];
            }
            // initialize the WiiChuk (pair)
            this.device.Write(new Byte[] { 0xF0, 0x55 });
            this.device.Write(new Byte[] { 0xFB, 0x00 });
            if (this.csPin != null)
            {
               this.csPin.Value = true;
               this.device.Write(new Byte[] { 0xF0, 0x55 });
               this.device.Write(new Byte[] { 0xFB, 0x00 });
            }
            this.task = Tpl.Task.Factory.StartNew(() => { });
            reactor.Poll(
               () => this.task.IsCompleted,
               () =>
               {
                  this.task = Tpl.Task.Factory.StartNew(
                     () =>
                     {
                        if (this.csPin != null)
                           this.csPin.Value = false;
                        this.device.Write(new Byte[1] { 0 });
                        Thread.Sleep(1);
                        this.device.Read(this.buffer, 0, WiiChukState.EncodedSize);
                        if (this.csPin != null)
                        {
                           this.csPin.Value = true;
                           this.device.Write(new Byte[1] { 0 });
                           Thread.Sleep(1);
                           this.device.Read(this.buffer, WiiChukState.EncodedSize, WiiChukState.EncodedSize);
                        }
                        if (this.Received != null)
                           this.Received(this.buffer);
                     }
                  );
               }
            );
         }
         catch (Exception)
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.device != null)
            this.device.Dispose();
         this.device = null;
         if (this.csPin != null)
            this.csPin.Dispose();
         this.csPin = null;
      }

      #region IWiiChukReceiver Implementation
      public Int32 Count
      {
         get
         {
            return this.buffer.Length / WiiChukState.EncodedSize;
         }
         set
         {
            if (value != this.buffer.Length / WiiChukState.EncodedSize)
               throw new NotSupportedException();
         }
      }
      public event Action<Byte[]> Received;
      #endregion
   }
}
