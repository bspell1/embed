using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using Mono.Unix;
using Mono.Unix.Native;

namespace NPi
{
   public static class Native
   {
      [Flags]
      public enum EventFDFlags : int
      {
         None = 0,
         EFD_SEMAPHORE = 1,
         EFD_NONBLOCK = OpenFlags.O_NONBLOCK
      }

      #region Math Interface
      public static Single Atan2 (Single y, Single x)
      {
         Single result;
         Atan2(ref y, ref x, out result);
         return result;
      }
      public static Double Atan2 (Double y, Double x)
      {
         Double result;
         Atan2(ref y, ref x, out result);
         return result;
      }
      [DllImport("monoext", EntryPoint = "MonoMathAtan2F")]
      private static extern void Atan2 (ref Single y, ref Single x, out Single result);
      [DllImport("monoext", EntryPoint = "MonoMathAtan2D")]
      private static extern void Atan2 (ref Double y, ref Double x, out Double result);
      #endregion

      #region "I2C interface"
      [DllImport("monoext", EntryPoint = "I2cSetSlave", SetLastError = true)]
      public static extern Int32 I2cSetSlave (
         Int32 fd,
         Int32 address
      );
      [DllImport(
         "monoext",
         EntryPoint = "I2cSendReceive",
         SetLastError = true)]
      public static extern Int32 I2cSendReceive (
         Int32 fd,
         Int32 addr,
         IntPtr pTxBuffer,
         Int32 cbTxOffset,
         Int32 cbTxLength,
         IntPtr pRxBuffer,
         Int32 cbRxOffset,
         Int32 cbRxLength
      );
      #endregion

      #region "SPI Interface"
      [DllImport("monoext", EntryPoint = "SpiGetMode", SetLastError = true)]
      public static extern Int32 SpiGetMode (
         Int32 fd,
         out Byte pbMode
      );
      [DllImport("monoext", EntryPoint = "SpiSetMode", SetLastError = true)]
      public static extern Int32 SpiSetMode (
         Int32 fd,
         Byte bMode
      );
      [DllImport("monoext", EntryPoint = "SpiGetBitsPerWord", SetLastError = true)]
      public static extern Int32 SpiGetBitsPerWord (
         Int32 fd,
         out Byte pbBitsPerWord
      );
      [DllImport("monoext", EntryPoint = "SpiSetBitsPerWord", SetLastError = true)]
      public static extern Int32 SpiSetBitsPerWord (
         Int32 fd,
         Byte bBitsPerWord
      );
      [DllImport("monoext", EntryPoint = "SpiGetClockSpeed", SetLastError = true)]
      public static extern Int32 SpiGetClockSpeed (
         Int32 fd,
         out Int32 pnClockSpeed
      );
      [DllImport("monoext", EntryPoint = "SpiSetClockSpeed", SetLastError = true)]
      public static extern Int32 SpiSetClockSpeed (
         Int32 fd,
         Int32 nClockSpeed
      );
      [DllImport("monoext", EntryPoint = "SpiSendReceive", SetLastError = true)]
      public static extern Int32 SpiSendReceive (
         Int32 fd,
         IntPtr txBuffer,
         Int32 txOffset,
         IntPtr rxBuffer,
         Int32 rxOffset,
         Int32 count
      );
      #endregion

      #region "Eventfd Interface"
      [DllImport("monoext", EntryPoint = "EventFDCreate", SetLastError = true)]
      public static extern Int32 EventFDCreate (Int32 value, EventFDFlags flags);
      #endregion
   }
}
