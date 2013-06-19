using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public class Nrf24Receiver : IWiiChukReceiver
   {
      private Nrf24 receiver;
      private Int32 pipe;
      private Byte[] buffer;

      public Nrf24Receiver (Nrf24 receiver, String address, Int32 pipe = 0)
      {
         this.pipe = pipe;
         this.receiver = receiver;
         this.receiver.SetRXAddress(pipe, address);
         this.receiver.RXDataReady += status =>
         {
            while (status.RXReadyPipe == this.pipe)
            {
               this.receiver.ReceivePacket(this.buffer);
               if (this.Received != null)
                  this.Received(this.buffer);
               status = this.receiver.Status;
            }
         };
         this.Count = 1;
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
            if (this.buffer == null || value != this.buffer.Length / WiiChukState.EncodedSize)
            {
               this.receiver.SetRXLength(
                  this.pipe, 
                  value * WiiChukState.EncodedSize
               );
               this.buffer = new Byte[value * WiiChukState.EncodedSize];
            }
         }
      }
      public event Action<Byte[]> Received;
      #endregion
   }
}
