using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.Psx
{
   public class Nrf24Receiver : IPsxPadReceiver, IDisposable
   {
      private Nrf24 receiver;
      private Int32 pipe;
      private Byte[] buffer;

      public Nrf24Receiver (Nrf24 receiver, String address, Int32 pipe = 0)
      {
         this.buffer = new Byte[PsxPadState.EncodedSizeAnalog];
         this.pipe = pipe;
         this.receiver = receiver;
         this.receiver.SetRXAddress(pipe, address);
         this.receiver.SetRXLength(pipe, PsxPadState.EncodedSizeAnalog);
         var ack = new Nrf24.PipeFlagRegister(this.receiver.AutoAck);
         ack[pipe] = false;
         this.receiver.AutoAck = ack;
         this.receiver.RXDataReady += status =>
         {
            if (status.RXReadyPipe == this.pipe)
            {
               this.receiver.ReceivePacket(this.buffer);
               if (this.Received != null)
                  this.Received(PsxPadState.Decode(this.buffer, 0, this.buffer.Length));
            }
         };
      }
      public void Dispose ()
      {
      }

      #region IWiiChukReceiver Implementation
      public event Action<PsxPadState> Received;
      #endregion
   }
}
