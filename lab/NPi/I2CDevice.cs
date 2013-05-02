using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using Mono.Unix;
using Mono.Unix.Native;

namespace NPi
{
   public class I2CDevice : IDisposable
   {
      private Int32 fd = -1;

      public I2CDevice (String path)
      {
         this.fd = Syscall.open(path, OpenFlags.O_RDWR);
         if (fd < 0)
            throw new Win32Exception();
      }

      public void Dispose ()
      {
         if (this.fd >= 0)
            Syscall.close(this.fd);
         this.fd = -1;
      }

      public Int32 SlaveAddress
      {
         get; set; 
      }

      public void Read (Byte[] buffer)
      {
         Read(buffer, 0, buffer.Length);
      }
      public void Read (Byte[] buffer, Int32 count)
      {
         Read(buffer, 0, count);
      }

      public Int32 Read (Byte[] buffer, Int32 offset, Int32 count)
      {
         GCHandle hBuffer = GCHandle.Alloc(buffer, GCHandleType.Pinned);
         try
         {
            if (SetSlaveAddress(this.fd, this.SlaveAddress) < 0)
               throw new Win32Exception();
            var result = Syscall.read(
               this.fd, 
               hBuffer.AddrOfPinnedObject() + offset, 
               (UInt64)count
            );
            if (result < 0)
               throw new Win32Exception();
            return (Int32)result;
         }
         finally
         {
            hBuffer.Free();
         }
      }

      public void Write (Byte[] buffer)
      {
         Write(buffer, 0, buffer.Length);
      }

      public void Write (Byte[] buffer, Int32 count)
      {
         Write(buffer, 0, count);
      }

      public void Write (Byte[] buffer, Int32 offset, Int32 count)
      {
         GCHandle hBuffer = GCHandle.Alloc(buffer, GCHandleType.Pinned);
         try
         {
            if (SetSlaveAddress(this.fd, this.SlaveAddress) < 0)
               throw new Win32Exception();
            var result = Syscall.write(
               this.fd,
               hBuffer.AddrOfPinnedObject() + offset,
               (UInt64)count
            );
            if (result != count)
               throw new Win32Exception();
         }
         finally
         {
            hBuffer.Free();
         }
      }

      #region Native Methods
      [DllImport("monoext", EntryPoint = "i2c_set_slave", SetLastError = true)]
      private static extern Int32 SetSlaveAddress (
         Int32 fd, 
         Int32 address
      );
      #endregion
   }
}
