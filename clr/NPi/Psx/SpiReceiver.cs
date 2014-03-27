using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using Tpl = System.Threading.Tasks;

namespace NPi.Psx
{
   public class SpiReceiver : IPsxPadReceiver, IDisposable
   {
      private SpiDevice device;
      private Byte[] buffer;
      private Tpl.Task task;

      public SpiReceiver (String spiPath, Reactor reactor, Int32 ssPin = -1)
      {
         try
         {
            this.device = new SpiDevice(spiPath, ssPin);
            this.device.LsbFirst = true;
            this.device.ClockSpeed = 100000;
            this.device.Mode = SpiMode.CPHA | SpiMode.CPOL;
            // enter config mode
            this.device.Write(new Byte[] { 0x01, 0x43, 0x00, 0x01 });
            // switch to analog mode
            this.device.Write(new Byte[] { 0x01, 0x44, 0x00, 0x01, 0x00 });
            // exit config mode
            this.device.Write(new Byte[] { 0x01, 0x43, 0x00, 0x00 });
            // start the polling task
            this.buffer = new Byte[PsxPadState.EncodedSizeAnalog + 3];
            this.task = Tpl.Task.Factory.StartNew(() => { });
            reactor.Poll(
               () => this.task.IsCompleted,
               () =>
               {
                  this.task = Tpl.Task.Factory.StartNew(
                     () =>
                     {
                        this.buffer[0] = 0x01;
                        this.buffer[1] = 0x42;
                        this.buffer[2] = 0x00;
                        this.device.ReadWrite(this.buffer);
                        if (this.Received != null)
                           this.Received(
                              PsxPadState.Decode(
                                 this.buffer, 
                                 3, 
                                 PsxPadState.EncodedSizeAnalog
                              )
                           );
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
      }

      #region IWiiChukReceiver Implementation
      public event Action<PsxPadState> Received;
      #endregion
   }
}
