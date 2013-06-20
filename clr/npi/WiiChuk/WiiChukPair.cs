using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public class WiiChukPair : IDisposable
   {
      private WiiChukState LeftZero = WiiChukState.Zero;
      private WiiChukState RightZero = WiiChukState.Zero;
      private IWiiChukReceiver receiver;
      public event Action<WiiChukState, WiiChukState> Updated;

      public WiiChukPair (IWiiChukReceiver receiver)
      {
         this.receiver = receiver;
         this.receiver.Count = 2;
         this.receiver.Received += data =>
         {
            // decode the chuk values from the received buffer
            var left = WiiChukState.Decode(data, 0, WiiChukState.EncodedSize);
            var right = WiiChukState.Decode(data, WiiChukState.EncodedSize, WiiChukState.EncodedSize);
            // if all buttons pressed, recalibrate
            if (left.CButton && right.CButton && left.ZButton && right.ZButton)
            {
               this.LeftZero = left;
               this.RightZero = right;
               left.CButton = left.ZButton = right.CButton = right.ZButton = false;
            }
            // calibrate the results and dispatch the updated event
            left = this.Left = left.Calibrate(this.LeftZero);
            right = this.Right = right.Calibrate(this.RightZero);
            if (this.Updated != null)
               this.Updated(left, right);
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
