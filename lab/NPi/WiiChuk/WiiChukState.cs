using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public struct WiiChukState
   {
      public const Int32 EncodedSize = 6;
      private Boolean lastCButton;
      private Boolean lastZButton;

      public Int32 JoystickX { get; private set; }
      public Int32 JoystickY { get; private set; }
      public Int32 AccelerometerX { get; private set; }
      public Int32 AccelerometerY { get; private set; }
      public Int32 AccelerometerZ { get; private set; }
      public Boolean CButton { get; private set; }
      public Boolean ZButton { get; private set; }
      public Boolean LastCButton
      {
         get
         {
            var c = this.lastCButton;
            this.lastCButton = false;
            return c;
         }
      }
      public Boolean LastZButton
      {
         get
         {
            var z = this.lastZButton;
            this.lastZButton = false;
            return z;
         }
      }

      public static WiiChukState Decode (Byte[] data, Int32 offset, Int32 count)
      {
         var chuk = new WiiChukState()
         {
            JoystickX = data[offset + 0],
            JoystickY = data[offset + 1],
            AccelerometerX = ((Int32)data[offset + 2] << 2) | ((data[offset + 5] >> 2) & 0x3),
            AccelerometerY = ((Int32)data[offset + 3] << 2) | ((data[offset + 5] >> 4) & 0x3),
            AccelerometerZ = ((Int32)data[offset + 4] << 2) | ((data[offset + 5] >> 6) & 0x3),
            CButton = ((data[offset + 5] >> 1) & 0x1) == 0,
            ZButton = ((data[offset + 5] >> 0) & 0x1) == 0
         };
         if (chuk.CButton)
            chuk.lastCButton = true;
         if (chuk.ZButton)
            chuk.lastZButton = true;
         return chuk;
      }
   }
}
