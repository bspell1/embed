using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using Mono.Unix;
using Mono.Unix.Native;

namespace NPi
{
   [Flags]
   public enum SpiMode : byte
   {
      CPHA = 0x01,
      CPOL = 0x02,
      Mode0 = 0x00,
      Mode1 = CPHA,
      Mode2 = CPOL,
      Mode3 = CPHA | CPOL
   }

   public class SpiDevice : IDisposable
   {
      static Byte[] BitReverse =
      {
         0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
         0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
         0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
         0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
         0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
         0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
         0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
         0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
         0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
         0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
         0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
         0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
         0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
         0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
         0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
         0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
      };

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
            {
               this.ss = new Gpio(ssPin, Gpio.Mode.Write);
               this.ss.Value = true;
            }
            this.Mode = SpiMode.Mode0;
            this.BitsPerWord = 8;
            this.ClockSpeed = 2000000;
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

      public SpiMode Mode
      {
         get
         { 
            Byte value;
            if (GetMode(this.fd, out value) < 0)
               throw new Win32Exception();
            return (SpiMode)value;
         }
         set
         {
            if (SetMode(this.fd, (Byte)value) < 0)
               throw new Win32Exception();
         }
      }

      public Int32 BitsPerWord
      {
         get
         {
            Byte value;
            if (GetBitsPerWord(this.fd, out value) < 0)
               throw new Win32Exception();
            return value;
         }
         set
         {
            if (SetBitsPerWord(this.fd, (Byte)value) < 0)
               throw new Win32Exception();
         }
      }

      public Int32 ClockSpeed
      {
         get
         {
            Int32 value;
            if (GetClockSpeed(this.fd, out value) < 0)
               throw new Win32Exception();
            return value;
         }
         set
         {
            if (SetClockSpeed(this.fd, value) < 0)
               throw new Win32Exception();
         }
      }

      public Boolean LsbFirst
      {
         get; set;
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
         if (txBuffer != null && txOffset + count > txBuffer.Length)
            throw new ArgumentException("txOffset");
         if (rxBuffer == null && rxOffset != 0)
            throw new ArgumentException("txOffset");
         if (rxBuffer != null && rxOffset + count > rxBuffer.Length)
            throw new ArgumentException("rxOffset");
         if (count > 0)
         {
            GCHandle? hTx = null, hRx = null;
            if (txBuffer != null && this.LsbFirst)
               for (var i = 0; i < count; i++)
                  txBuffer[txOffset + i] = BitReverse[txBuffer[txOffset + i]];
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
               if (rxBuffer != null && this.LsbFirst && rxBuffer != txBuffer)
                  for (var i = 0; i < count; i++)
                     rxBuffer[rxOffset + i] = BitReverse[rxBuffer[rxOffset + i]];
            }
            finally
            {
               if (this.ss != null)
                  this.ss.Value = true;
               if (hTx != null)
                  hTx.Value.Free();
               if (hRx != null)
                  hRx.Value.Free();
               if (txBuffer != null && this.LsbFirst)
                  for (var i = 0; i < count; i++)
                     txBuffer[txOffset + i] = BitReverse[txBuffer[txOffset + i]];
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

      public void ReadWrite (Byte[] buffer, Int32 offset, Int32 length)
      {
         ReadWrite(buffer, offset, buffer, offset, length);
      }

      public void ReadWrite (Byte[] buffer, Int32 offset)
      {
         ReadWrite(buffer, offset, buffer, offset);
      }

      public void ReadWrite (Byte[] buffer)
      {
         ReadWrite(buffer, buffer);
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
      [DllImport("monoext", EntryPoint = "SpiGetMode", SetLastError = true)]
      private static extern Int32 GetMode (
         Int32 fd,
         out Byte pbMode
      );
      [DllImport("monoext", EntryPoint = "SpiSetMode", SetLastError = true)]
      private static extern Int32 SetMode (
         Int32 fd, 
         Byte bMode
      );
      [DllImport("monoext", EntryPoint = "SpiGetBitsPerWord", SetLastError = true)]
      private static extern Int32 GetBitsPerWord (
         Int32 fd,
         out Byte pbBitsPerWord
      );
      [DllImport("monoext", EntryPoint = "SpiSetBitsPerWord", SetLastError = true)]
      private static extern Int32 SetBitsPerWord (
         Int32 fd,
         Byte bBitsPerWord
      );
      [DllImport("monoext", EntryPoint = "SpiGetClockSpeed", SetLastError = true)]
      private static extern Int32 GetClockSpeed (
         Int32 fd,
         out Int32 pnClockSpeed
      );
      [DllImport("monoext", EntryPoint = "SpiSetClockSpeed", SetLastError = true)]
      private static extern Int32 SetClockSpeed (
         Int32 fd,
         Int32 nClockSpeed
      );
      [DllImport("monoext", EntryPoint = "SpiSendReceive", SetLastError = true)]
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
