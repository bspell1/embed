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
            if (Native.I2cSetSlave(this.fd, this.SlaveAddress) < 0)
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
            if (Native.I2cSetSlave(this.fd, this.SlaveAddress) < 0)
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

      public void ReadWrite (Byte[] txBuffer, Byte[] rxBuffer)
      {
         ReadWrite(txBuffer, 0, rxBuffer, 0);
      }

      public void ReadWrite (
         Byte[] txBuffer,
         Int32 txOffset,
         Byte[] rxBuffer,
         Int32 rxOffset)
      {
         if (txBuffer == null)
            throw new ArgumentNullException("txBuffer");
         if (rxBuffer == null)
            throw new ArgumentNullException("rxBuffer");
         if (txOffset < 0 || txOffset >= txBuffer.Length)
            throw new ArgumentException("txOffset");
         if (txOffset < 0 || rxOffset >= rxBuffer.Length)
            throw new ArgumentException("rxOffset");
         ReadWrite(
            txBuffer,
            txOffset,
            txBuffer.Length,
            rxBuffer,
            rxOffset,
            rxBuffer.Length
         );
      }

      public void ReadWrite (
         Byte[] txBuffer,
         Int32 txOffset,
         Int32 txCount,
         Byte[] rxBuffer,
         Int32 rxOffset,
         Int32 rxCount)
      {
         if (txBuffer == null)
            throw new ArgumentNullException("txBuffer");
         if (rxBuffer == null)
            throw new ArgumentNullException("rxBuffer");
         if (txOffset < 0)
            throw new ArgumentOutOfRangeException("txOffset");
         if (rxOffset < 0)
            throw new ArgumentOutOfRangeException("rxOffset");
         if (txCount <= 0)
            throw new ArgumentOutOfRangeException("txCount");
         if (rxCount <= 0)
            throw new ArgumentOutOfRangeException("rxCount");
         if (txOffset + txCount > txBuffer.Length)
            throw new ArgumentException("txOffset");
         if (rxOffset + rxCount > rxBuffer.Length)
            throw new ArgumentException("rxOffset");
         GCHandle? hTx = null, hRx = null;
         try
         {
            IntPtr pTx = IntPtr.Zero;
            IntPtr pRx = IntPtr.Zero;
            if (txBuffer != null)
            {
               hTx = GCHandle.Alloc(txBuffer, GCHandleType.Pinned);
               pTx = hTx.Value.AddrOfPinnedObject();
            }
            if (rxBuffer != null)
            {
               hRx = GCHandle.Alloc(rxBuffer, GCHandleType.Pinned);
               pRx = hRx.Value.AddrOfPinnedObject();
            }
            if (Native.I2cSetSlave(this.fd, this.SlaveAddress) < 0)
               throw new Win32Exception();
            if (Native.I2cSendReceive(
                  this.fd, 
                  this.SlaveAddress, 
                  pTx, 
                  txOffset, 
                  txCount, 
                  pRx, 
                  rxOffset, 
                  rxCount) < 0)
               throw new Win32Exception();
         }
         finally
         {
            if (hTx != null)
               hTx.Value.Free();
            if (hRx != null)
               hRx.Value.Free();
         }
      }
   }
}
