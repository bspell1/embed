using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using Mono.Unix;
using Mono.Unix.Native;

namespace NPi
{
   public class SpiDevice : IDisposable
   {
      private Int32 fd = -1;
      private Gpio ss = null;

      public SpiDevice (String path) 
         : this(path, -1)
      {
      }
      public SpiDevice (String path, Int32 ssPin)
      {
         try
         {
            this.fd = Syscall.open(path, OpenFlags.O_RDWR);
            if (this.fd < 0)
               throw new Win32Exception();
            if (ssPin != -1)
               this.ss = new Gpio(ssPin, Gpio.Mode.Write);
            this.ss.Value = true;
         }
         catch
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.ss != null)
            this.ss.Dispose();
         this.ss = null;
         if (this.fd >= 0)
            Syscall.close(this.fd);
         this.fd = -1;
      }

      public void ReadWrite (
         Byte[] txBuffer, 
         Int32 txOffset,
         Byte[] rxBuffer, 
         Int32 rxOffset,
         Int32 count)
      {
         if (count < 0)
            throw new ArgumentOutOfRangeException("count");
         if (txOffset < 0)
            throw new ArgumentOutOfRangeException("txOffset");
         if (rxOffset < 0)
            throw new ArgumentOutOfRangeException("rxOffset");
         if (txBuffer == null && rxBuffer == null)
            throw new ArgumentNullException("txBuffer");
         if (txBuffer == null && txOffset != 0)
            throw new ArgumentException("txOffset");
         if (txBuffer != null && txOffset + count > rxBuffer.Length)
            throw new ArgumentException("txOffset");
         if (rxBuffer == null && rxOffset != 0)
            throw new ArgumentException("txOffset");
         if (rxBuffer != null && rxOffset + count > rxBuffer.Length)
            throw new ArgumentException("rxOffset");
         if (count > 0)
         {
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
               if (this.ss != null)
                  this.ss.Value = false;
               if (SendReceive(this.fd, pTx, txOffset, pRx, rxOffset, count) < 0)
                  throw new Win32Exception();
            }
            finally
            {
               if (this.ss != null)
                  this.ss.Value = true;
               if (hTx != null)
                  hTx.Value.Free();
               if (hRx != null)
                  hRx.Value.Free();
            }
         }
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
         if (txBuffer.Length - txOffset != rxBuffer.Length - rxOffset)
            throw new ArgumentException("txBuffer.Length");
         ReadWrite(
            txBuffer, 
            txOffset, 
            rxBuffer, 
            rxOffset, 
            txBuffer.Length - txOffset
         );
      }

      public void ReadWrite (Byte[] txBuffer, Byte[] rxBuffer)
      {
         ReadWrite(txBuffer, 0, rxBuffer, 0);
      }

      public void Read (Byte[] buffer, Int32 offset, Int32 count)
      {
         ReadWrite(null, 0, buffer, offset, count);
      }

      public void Read (Byte[] buffer, Int32 count)
      {
         ReadWrite(null, 0, buffer, 0, count);
      }

      public void Read (Byte[] buffer)
      {
         if (buffer == null)
            throw new ArgumentNullException("buffer");
         ReadWrite(null, 0, buffer, 0, buffer.Length);
      }

      public void Write (Byte[] buffer, Int32 offset, Int32 count)
      {
         if (buffer == null)
            throw new ArgumentNullException("buffer");
         ReadWrite(buffer, offset, null, 0, count);
      }

      public void Write (Byte[] buffer, Int32 count)
      {
         if (buffer == null)
            throw new ArgumentNullException("buffer");
         ReadWrite(buffer, 0, null, 0, count);
      }

      public void Write (Byte[] buffer)
      {
         if (buffer == null)
            throw new ArgumentNullException("buffer");
         ReadWrite(buffer, 0, null, 0, buffer.Length);
      }

      #region Native Methods
      [DllImport("monoext", EntryPoint = "spi_send_recv", SetLastError = true)]
      private static extern Int32 SendReceive (
         Int32 fd, 
         IntPtr txBuffer, 
         Int32 txOffset,
         IntPtr rxBuffer, 
         Int32 rxOffset,
         Int32 count
      );
      #endregion
   }
}
