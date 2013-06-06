using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public class WiiChuk : IDisposable
   {
      IWiiChukReceiver receiver;
      public event Action<WiiChukState> Updated;

      public WiiChuk (IWiiChukReceiver receiver)
      {
         this.receiver = receiver;
         this.receiver.Count = 1;
         this.receiver.Received += data =>
         {
            this.State = WiiChukState.Decode(data, 0, data.Length);
            if (this.Updated != null)
               this.Updated(this.State);
         };
      }

      public void Dispose ()
      {
         if (this.receiver is IDisposable)
            ((IDisposable)this.receiver).Dispose();
         this.receiver = null;
      }

      public WiiChukState State
      {
         get; private set;
      }
   }
}
