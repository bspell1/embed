using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public class WiiChukPair : IDisposable
   {
      IWiiChukReceiver receiver;
      public event Action<WiiChukState, WiiChukState> Updated;

      public WiiChukPair (IWiiChukReceiver receiver)
      {
         this.receiver = receiver;
         this.receiver.Count = 2;
         this.receiver.Received += data =>
         {
            this.Left = WiiChukState.Decode(data, 0, WiiChukState.EncodedSize);
            this.Right = WiiChukState.Decode(data, WiiChukState.EncodedSize, WiiChukState.EncodedSize);
            if (this.Updated != null)
               this.Updated(this.Left, this.Right);
         };
      }

      public void Dispose ()
      {
         if (this.receiver is IDisposable)
            ((IDisposable)this.receiver).Dispose();
         this.receiver = null;
      }

      public WiiChukState Left
      {
         get; private set;
      }
      public WiiChukState Right
      {
         get; private set;
      }
   }
}
