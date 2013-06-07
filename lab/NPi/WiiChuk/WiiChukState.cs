using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public struct WiiChukState
   {
      public const Int32 EncodedSize = 6;
      private const Int32 JoystickMin = 30;
      private const Int32 JoystickMax = 225;
      private const Int32 JoystickRange = JoystickMax - JoystickMin;
      private const Int32 AcceleroMin = 1;
      private const Int32 AcceleroMax = 1023;
      private const Int32 AcceleroRange = AcceleroMax - AcceleroMin;

      public Single JoystickX { get; private set; }
      public Single JoystickY { get; private set; }
      public Single AcceleroX { get; private set; }
      public Single AcceleroY { get; private set; }
      public Single AcceleroZ { get; private set; }
      public Boolean CButton { get; private set; }
      public Boolean ZButton { get; private set; }

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
         // clamp the readings to [0, range]
         jx = Math.Min(Math.Max(jx, JoystickMin), JoystickMax) - JoystickMin;
         jy = Math.Min(Math.Max(jy, JoystickMin), JoystickMax) - JoystickMin;
         ax = Math.Min(Math.Max(ax, AcceleroMin), AcceleroMax) - AcceleroMin;
         ay = Math.Min(Math.Max(ay, AcceleroMin), AcceleroMax) - AcceleroMin;
         az = Math.Min(Math.Max(az, AcceleroMin), AcceleroMax) - AcceleroMin;
         // normalize the readings and return the chuk state
         return new WiiChukState()
         {
            JoystickX = 2f * jx / JoystickRange - 1,
            JoystickY = 2f * jy / JoystickRange - 1,
            AcceleroX = 2f * ax / AcceleroRange - 1,
            AcceleroY = 2f * ay / AcceleroRange - 1,
            AcceleroZ = 2f * az / AcceleroRange - 1,
            CButton = bc,
            ZButton = bz
         };
      }
   }
}
