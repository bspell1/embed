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
      private UnixStream stream;
      private Int32 slave = 0;

      public enum Access
      {
         Read = OpenFlags.O_RDONLY,
         Write = OpenFlags.O_WRONLY,
         ReadWrite = OpenFlags.O_RDWR
      }

      public I2CDevice (String path) 
         : this(path, Access.ReadWrite)
      {
      }
      public I2CDevice (
         String path, 
         Access access)
      {
         Int32 fd = Syscall.open(path, (OpenFlags)access);
         if (fd < 0)
            throw new Win32Exception();
         this.stream = new UnixStream(fd);
      }

      public void Dispose ()
      {
         if (this.stream != null)
            this.stream.Dispose();
         this.stream = null;
      }

      public Int32 SlaveAddress
      {
         get { return this.slave; }
         set
         {
            if (SetSlaveAddress(this.stream.Handle, value) < 0)
               throw new Win32Exception();
            this.slave = value;
         }
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
         return this.stream.Read(buffer, offset, count);
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
         this.stream.Write(buffer, offset, count);
      }

      #region Native Methods
      [DllImport("monoext", EntryPoint = "i2c_set_slave", SetLastError = true)]
      private static extern Int32 SetSlaveAddress (
         Int32 fd, 
         Int32 address);
      #endregion
   }
}
