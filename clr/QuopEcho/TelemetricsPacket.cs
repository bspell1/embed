using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuopEcho
{
   public struct TelemetricsPacket
   {
      public const Int32 EncodedSize = 16;

      public Int32 RollAngle { get; private set; }
      public Int32 PitchAngle { get; private set; }
      public Int32 YawRate { get; private set; }
      public Int32 ThrustInput { get; private set; }
      public Int32 RollInput { get; private set; }
      public Int32 PitchInput { get; private set; }
      public Int32 YawInput { get; private set; }
      public Int32 BowRotor { get; private set; }
      public Int32 SternRotor { get; private set; }
      public Int32 PortRotor { get; private set; }
      public Int32 StarboardRotor { get; private set; }
      public Int32 Counter { get; private set; }

      public static TelemetricsPacket Decode (Byte[] encoded)
      {
         var idx = 0;
         return new TelemetricsPacket()
         {
            RollAngle = (SByte)encoded[idx++],
            PitchAngle = (SByte)encoded[idx++],
            YawRate = (SByte)encoded[idx++],
            ThrustInput = encoded[idx++],
            RollInput = (SByte)encoded[idx++],
            PitchInput = (SByte)encoded[idx++],
            YawInput = (SByte)encoded[idx++],
            BowRotor = (Int16)((UInt16)encoded[idx++] | ((Int16)encoded[idx++] << 8)),
            SternRotor = (Int16)((UInt16)encoded[idx++] | ((Int16)encoded[idx++] << 8)),
            PortRotor = (Int16)((UInt16)encoded[idx++] | ((Int16)encoded[idx++] << 8)),
            StarboardRotor = (Int16)((UInt16)encoded[idx++] | ((Int16)encoded[idx++] << 8)),
            Counter = encoded[idx++]
         };
      }
   }
}
