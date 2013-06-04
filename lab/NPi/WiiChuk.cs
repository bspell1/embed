using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi
{
   public class WiiChuk : IDisposable
   {
      private Gpio csPin;
      private Boolean csValue;
      private I2CDevice device;
      private Byte[] writeBuffer = new Byte[] { 0x00 };
      private Byte[] readBuffer = new Byte[6];
      private Boolean lastCButton;
      private Boolean lastZButton;

      public WiiChuk (String i2cPath, Int32 csPin = -1, Boolean csValue = false)
      {
         try
         {
            this.device = new I2CDevice(i2cPath);
            this.device.SlaveAddress = 0x52;
            if (csPin != -1)
            {
               this.csPin = new Gpio(csPin, Gpio.Mode.Write);
               this.csValue = csValue;
               this.csPin.Value = !this.csValue;
            }
            // initialize the WiiChuk
            this.device.Write(new Byte[] { 0xF0, 0x55 });
            this.device.Write(new Byte[] { 0xFB, 0x00 });
         }
         catch
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.device != null)
            this.device.Dispose();
         this.device = null;
         if (this.csPin != null)
            this.csPin.Dispose();
         this.csPin = null;
      }

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

      public void Read ()
      {
         if (this.csPin != null)
            this.csPin.Value = this.csValue;
         try
         {
            var result = this.readBuffer;
            //this.device.ReadWrite(this.writeBuffer, result);
            this.device.Write(this.writeBuffer);
            System.Threading.Thread.Sleep(1);
            this.device.Read(result);
            //for (var i = 0; i < result.Length; i++)
               //result[i] = (Byte)((result[i] ^ 0x17) + 0x17);
            this.JoystickX = result[0];
            this.JoystickY = result[1];
            this.AccelerometerX = ((Int32)result[2] << 2) | ((result[5] >> 2) & 0x3);
            this.AccelerometerY = ((Int32)result[3] << 2) | ((result[5] >> 4) & 0x3);
            this.AccelerometerZ = ((Int32)result[4] << 2) | ((result[5] >> 6) & 0x3);
            this.CButton = ((result[5] >> 1) & 0x1) == 0;
            this.ZButton = ((result[5] >> 0) & 0x1) == 0;
            if (this.CButton)
               this.lastCButton = true;
            if (this.ZButton)
               this.lastZButton = true;
         }
         finally
         {
            if (this.csPin != null)
               this.csPin.Value = !this.csValue;
         }
      }
   }
}
