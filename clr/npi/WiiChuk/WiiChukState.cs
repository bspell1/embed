using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public struct WiiChukState
   {
      public const Int32 EncodedSize = 6;
      public static readonly WiiChukState Zero = new WiiChukState();
      private const Int32 JoystickMin = 30;
      private const Int32 JoystickMax = 225;
      private const Int32 JoystickRange = JoystickMax - JoystickMin;
      private const Int32 AcceleroMin = 1;
      private const Int32 AcceleroMax = 1023;
      private const Int32 AcceleroRange = AcceleroMax - AcceleroMin;

      public Single JoystickX { get; set; }
      public Single JoystickY { get; set; }
      public Single AcceleroX { get; set; }
      public Single AcceleroY { get; set; }
      public Single AcceleroZ { get; set; }
      public Boolean CButton { get; set; }
      public Boolean ZButton { get; set; }

      public static WiiChukState Decode (Byte[] data, Int32 offset, Int32 count)
      {
         // decode the readings from the buffer
         var jx = (Int32)data[offset + 0];
         var jy = (Int32)data[offset + 1];
         var ax = ((Int32)data[offset + 2] << 2) | ((data[offset + 5] >> 2) & 0x3);
         var ay = ((Int32)data[offset + 3] << 2) | ((data[offset + 5] >> 4) & 0x3);
         var az = ((Int32)data[offset + 4] << 2) | ((data[offset + 5] >> 6) & 0x3);
         var bc = ((data[offset + 5] >> 1) & 0x1) == 0;
         var bz = ((data[offset + 5] >> 0) & 0x1) == 0;
         // normalize the readings to [-1,1] and return the chuk state
         return new WiiChukState()
         {
            JoystickX = 2f * (jx - JoystickMin) / JoystickRange - 1f,
            JoystickY = 2f * (jy - JoystickMin) / JoystickRange - 1f,
            AcceleroX = 2f * (ax - AcceleroMin) / AcceleroRange - 1f,
            AcceleroY = 2f * (ay - AcceleroMin) / AcceleroRange - 1f,
            AcceleroZ = 2f * (az - AcceleroMin) / AcceleroRange - 1f,
            CButton = bc,
            ZButton = bz
         };
      }

      public WiiChukState Calibrate (WiiChukState zero)
      {
         // offset joystick/accelerometer readings and clamp to [-1,1]
         return new WiiChukState()
         {
            JoystickX = Math.Min(Math.Max(this.JoystickX - zero.JoystickX, -1f), 1f),
            JoystickY = Math.Min(Math.Max(this.JoystickY - zero.JoystickY, -1f), 1f),
            AcceleroX = Math.Min(Math.Max(this.AcceleroX - zero.AcceleroX, -1f), 1f),
            AcceleroY = Math.Min(Math.Max(this.AcceleroY - zero.AcceleroY, -1f), 1f),
            AcceleroZ = Math.Min(Math.Max(this.AcceleroZ - zero.AcceleroZ, -1f), 1f),
            CButton = this.CButton,
            ZButton = this.ZButton
         };
      }
   }
}
