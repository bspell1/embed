using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NPi.Psx
{
   public struct PsxPadState
   {
      // buffer lengths
      public const Int32 EncodedSizeDigital = 2;
      public const Int32 EncodedSizeAnalog = 6;
      // joystick minimum move
      public const Single JoystickEpsilon = 0.001f;
      // digital buttons
      public Boolean Select { get; private set; }
      public Boolean L3 { get; private set; }
      public Boolean R3 { get; private set; }
      public Boolean Start { get; private set; }
      public Boolean Up { get; private set; }
      public Boolean Right { get; private set; }
      public Boolean Down { get; private set; }
      public Boolean Left { get; private set; }
      public Boolean L2 { get; private set; }
      public Boolean R2 { get; private set; }
      public Boolean L1 { get; private set; }
      public Boolean R1 { get; private set; }
      public Boolean Triangle { get; private set; }
      public Boolean Circle { get; private set; }
      public Boolean Cross { get; private set; }
      public Boolean Square { get; private set; }
      // analog joysticks
      public Single RX { get; private set; }
      public Single RY { get; private set; }
      public Single LX { get; private set; }
      public Single LY { get; private set; }

      public static PsxPadState Decode (Byte[] buffer, Int32 offset, Int32 count)
      {
         if (buffer == null)
            throw new ArgumentNullException("buffer");
         if (count != EncodedSizeDigital && count != EncodedSizeAnalog)
            throw new ArgumentException("count");
         if (offset < 0 || offset + count > buffer.Length)
            throw new ArgumentException("offset");
         return new PsxPadState()
         {
            // decode digital states
            Select = (buffer[offset + 0] & 0x01) == 0,
            L3 = (buffer[offset + 0] & 0x02) == 0,
            R3 = (buffer[offset + 0] & 0x04) == 0,
            Start = (buffer[offset + 0] & 0x08) == 0,
            Up = (buffer[offset + 0] & 0x10) == 0,
            Right = (buffer[offset + 0] & 0x20) == 0,
            Down = (buffer[offset + 0] & 0x40) == 0,
            Left = (buffer[offset + 0] & 0x80) == 0,
            L2 = (buffer[offset + 1] & 0x01) == 0,
            R2 = (buffer[offset + 1] & 0x02) == 0,
            L1 = (buffer[offset + 1] & 0x04) == 0,
            R1 = (buffer[offset + 1] & 0x08) == 0,
            Triangle = (buffer[offset + 1] & 0x10) == 0,
            Circle = (buffer[offset + 1] & 0x20) == 0,
            Cross = (buffer[offset + 1] & 0x40) == 0,
            Square = (buffer[offset + 1] & 0x80) == 0,
            // decode analog states if retrieved
            RX = count > EncodedSizeDigital ? ((Single)buffer[offset + 2] - 128.0f) / 128.0f : 0.0f,
            RY = count > EncodedSizeDigital ? ((Single)buffer[offset + 3] - 128.0f) / 128.0f : 0.0f,
            LX = count > EncodedSizeDigital ? ((Single)buffer[offset + 4] - 128.0f) / 128.0f : 0.0f,
            LY = count > EncodedSizeDigital ? ((Single)buffer[offset + 5] - 128.0f) / 128.0f : 0.0f,
         };
      }

      public override String ToString ()
      {
         return String.Format(
            "Ljx={0,5:0.00} Ljy={1,5:0.00} Rjx={2,5:0.00} Rjy={3,5:0.00} U={4} R={5} D={6} L={7} Tri={8} Cir={9} Crs={10} Sqr={11} L1={12} L2={13} L3={14} R1={15} R2={16} R3={17} Select={18} Start={19}",
            this.LX,
            this.LY,
            this.RX,
            this.RY,
            this.Up ? 1 : 0,
            this.Right ? 1 : 0,
            this.Down ? 1 : 0,
            this.Left ? 1 : 0,
            this.Triangle ? 1 : 0,
            this.Circle ? 1 : 0,
            this.Cross ? 1 : 0,
            this.Square ? 1 : 0,
            this.L1 ? 1 : 0,
            this.L2 ? 1 : 0,
            this.L3 ? 1 : 0,
            this.R1 ? 1 : 0,
            this.R2 ? 1 : 0,
            this.R3 ? 1 : 0,
            this.Select ? 1 : 0,
            this.Start ? 1 : 0
         );
      }
   }
}
