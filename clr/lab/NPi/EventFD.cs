using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using Mono.Unix.Native;

namespace NPi
{
   public class EventFD : IDisposable
   {
      private static readonly Int64 PosOneValue = 1;
      private static readonly Int64 NegOneValue = -1;
      private static readonly IntPtr PosOne =
         GCHandle.Alloc(PosOneValue, GCHandleType.Pinned).AddrOfPinnedObject();
      private static readonly IntPtr NegOne = 
         GCHandle.Alloc(NegOneValue, GCHandleType.Pinned).AddrOfPinnedObject();

      public EventFD (Int32 value = 0)
      {
         this.FD = Native.EventFDCreate(0, Native.EventFDFlags.None);
         if (this.FD < 0)
            throw new Win32Exception();
      }

      public void Dispose ()
      {
         if (this.FD >= 0)
            Syscall.close(this.FD);
         this.FD = -1;
      }

      public Int32 FD { get; private set; }

      public void Increment ()
      {
         Syscall.write(this.FD, PosOne, 8);
      }

      public void Decrement ()
      {
         Syscall.write(this.FD, NegOne, 8);
      }
   }
}
