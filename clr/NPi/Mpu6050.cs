using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NPi
{
   public sealed class Mpu6050 : IDisposable
   {
      public enum I2cAddress
      {
         _0 = 0x68,
         _1 = 0x69
      }

      public enum FrameSync
      {
         Disabled = 0,
         Temp = 1,
         GyroX = 2,
         GyroY = 3,
         GyroZ = 4,
         AccelX = 5,
         AccelY = 6,
         AccelZ = 7
      }

      public enum LowPassFilter
      {
         _260Hz = 0,
         _180Hz = 1,
         _90Hz = 2,
         _45Hz = 3,
         _20Hz = 4,
         _10Hz = 5,
         _5Hz = 6
      }

      public enum GyroScale
      {
         _500DegPerSec = 0,
         _1000DegPerSec = 1,
         _2000DegPerSec = 2,
         _4000DegPerSec = 3
      }

      public enum AccelScale
      {
         _4G = 0,
         _8G = 1,
         _16G = 2,
         _32G = 3
      }

      public enum ClockSource
      {
         Internal8MHz = 0, 
         PllGyroX = 1,
         PllGyroY = 2,
         PllGyroZ = 3,
         External32kHz = 4,
         External19MHz = 5,
         None = 7
      }

      public enum CycleModeFrequency
      {
         _1_25Hz = 0,
         _5Hz = 1,
         _20Hz = 2,
         _40Hz = 3
      }

      private const Byte RegisterGeneralConfig = 0x1A;
      private const Byte RegisterGyroConfig = 0x1B;
      private const Byte RegisterAccelConfig = 0x1C;
      private const Byte RegisterAccelX = 0x3B;
      private const Byte RegisterAccelY = 0x3D;
      private const Byte RegisterAccelZ = 0x3F;
      private const Byte RegisterTemperature1 = 0x41;
      private const Byte RegisterGyroX = 0x43;
      private const Byte RegisterGyroY = 0x45;
      private const Byte RegisterGyroZ = 0x47;
      private const Byte RegisterPower1 = 0x6B;
      private const Byte RegisterPower2 = 0x6C;

      private const Single GyroRange = 32750;
      private const Single AccelRange = 32767;

      private I2CDevice i2c;
      private Byte[] buffer = new Byte[3];

      public Mpu6050 (String path, I2cAddress address = I2cAddress._0)
      {
         this.i2c = new I2CDevice(path);
         this.i2c.SlaveAddress = (Int32)address;
         Reset();
      }

      public void Dispose ()
      {
         if (this.i2c != null)
         {
            Sleep();
            this.i2c.Dispose();
         }
         this.i2c = null;
      }

      #region Registers
      public GeneralConfigRegister GeneralConfig
      {
         get { return new GeneralConfigRegister(Read(RegisterGeneralConfig, 1)); }
         set { Write(RegisterGeneralConfig, value.Encode()); }
      }
      public GyroConfigRegister GyroConfig
      {
         get { return new GyroConfigRegister(Read(RegisterGyroConfig, 1)); }
         set { Write(RegisterGyroConfig, value.Encode()); }
      }
      public AccelConfigRegister AccelConfig
      {
         get { return new AccelConfigRegister(Read(RegisterAccelConfig, 1)); }
         set { Write(RegisterAccelConfig, value.Encode()); }
      }
      public PowerConfigRegister PowerConfig
      {
         get { return new PowerConfigRegister(Read(RegisterPower1, 2)); }
         set { Write(RegisterPower1, value.Encode()); }
      }
      public SampleRegister AccelX
      {
         get { return new SampleRegister(Read(RegisterAccelX, 2), AccelRange); }
      }
      public SampleRegister AccelY
      {
         get { return new SampleRegister(Read(RegisterAccelY, 2), AccelRange); }
      }
      public SampleRegister AccelZ
      {
         get { return new SampleRegister(Read(RegisterAccelZ, 2), AccelRange); }
      }
      public TemperatureRegister Temperature
      {
         get { return new TemperatureRegister(Read(RegisterTemperature1, 2)); }
      }
      public SampleRegister GyroX
      {
         get { return new SampleRegister(Read(RegisterGyroX, 2), GyroRange); }
      }
      public SampleRegister GyroY
      {
         get { return new SampleRegister(Read(RegisterGyroY, 2), GyroRange); }
      }
      public SampleRegister GyroZ
      {
         get { return new SampleRegister(Read(RegisterGyroZ, 2), GyroRange); }
      }
      #endregion

      #region Operations
      public void Reset ()
      {
         try
         {
            this.PowerConfig = new PowerConfigRegister()
            {
               Reset = true
            };
         }
         catch
         {
            //---------------------------------------------------------------
            // DEVNOTE: bspell - 11/8/2013
            // eat all exceptions here, as the MPU6050 stops responding 
            // over I2C after a reset, causing the Linux I2C driver to fail,
            // but the state of the driver appears to be stable afterwards
            //---------------------------------------------------------------
         }
      }

      public void Sleep ()
      {
         this.PowerConfig = new PowerConfigRegister(this.PowerConfig)
         {
            Sleep = true
         };
      }
      public void Wake ()
      {
         this.PowerConfig = new PowerConfigRegister(this.PowerConfig)
         {
            Sleep = false
         };
      }
      #endregion

      #region Communication
      private Byte[] Read (Byte register, Int32 count)
      {
         this.buffer[0] = register;
         this.i2c.ReadWrite(this.buffer, 0, 1, this.buffer, 1, count);
         var result = new Byte[count];
         Array.Copy(this.buffer, 1, result, 0, count);
         return result;
      }
      private void Write (Byte register, Byte[] data)
      {
         this.buffer[0] = register;
         Array.Copy(data, 0, this.buffer, 1, data.Length);
         this.i2c.Write(this.buffer, 0, data.Length + 1);
      }
      #endregion

      public struct GeneralConfigRegister
      {
         public FrameSync FrameSync { get; set; }
         public LowPassFilter Filter { get; set; }

         public static readonly GeneralConfigRegister Default = new GeneralConfigRegister();

         public GeneralConfigRegister (Byte[] encoded)
            : this()
         {
            this.FrameSync = (FrameSync)((encoded[0] >> 3) & 0x7);
            this.Filter = (LowPassFilter)((encoded[0] >> 0) & 0x7);
         }
         public GeneralConfigRegister (GeneralConfigRegister other)
            : this(other.Encode())
         {
         }
         public Byte[] Encode ()
         {
            return new Byte[1]
            {
               (Byte)(
                  ((Byte)this.FrameSync & 0x7) << 3 | 
                  ((Byte)this.Filter & 0x7) << 0
               )
            };
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("FrameSync:     {0}", this.FrameSync));
            str.AppendLine(String.Format("Filter:        {0}", this.Filter));
            return str.ToString();
         }
      }

      public struct GyroConfigRegister
      {
         public Boolean SelfTestX { get; set; }
         public Boolean SelfTestY { get; set; }
         public Boolean SelfTestZ { get; set; }
         public GyroScale Scale { get; set; }

         public static readonly GyroConfigRegister Default = new GyroConfigRegister()
         {
            Scale = GyroScale._500DegPerSec
         };

         public GyroConfigRegister (Byte[] encoded) : this()
         {
            this.SelfTestX = (encoded[0] & (1 << 7)) != 0;
            this.SelfTestY = (encoded[0] & (1 << 6)) != 0;
            this.SelfTestZ = (encoded[0] & (1 << 5)) != 0;
            this.Scale = (GyroScale)((encoded[0] >> 3) & 0x3);
         }
         public GyroConfigRegister (GyroConfigRegister other)
            : this(other.Encode())
         {
         }
         public Byte[] Encode ()
         {
            return new Byte[1]
            {
               (Byte)(
                  (this.SelfTestX ? 1 << 7 : 0 ) | 
                  (this.SelfTestY ? 1 << 6 : 0 ) | 
                  (this.SelfTestZ ? 1 << 5 : 0 ) | 
                  ((Byte)this.Scale & 0x3) << 3

               )
            };
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("SelfTestX:     {0}", this.SelfTestX));
            str.AppendLine(String.Format("SelfTestY:     {0}", this.SelfTestY));
            str.AppendLine(String.Format("SelfTestZ:     {0}", this.SelfTestZ));
            str.AppendLine(String.Format("Scale:         {0}", this.Scale));
            return str.ToString();
         }
      }

      public struct AccelConfigRegister
      {
         public Boolean SelfTestX { get; set; }
         public Boolean SelfTestY { get; set; }
         public Boolean SelfTestZ { get; set; }
         public AccelScale Scale { get; set; }

         public static readonly AccelConfigRegister Default = new AccelConfigRegister()
         {
            Scale = AccelScale._4G
         };

         public AccelConfigRegister (Byte[] encoded)
            : this()
         {
            this.SelfTestX = (encoded[0] & (1 << 7)) != 0;
            this.SelfTestY = (encoded[0] & (1 << 6)) != 0;
            this.SelfTestZ = (encoded[0] & (1 << 5)) != 0;
            this.Scale = (AccelScale)((encoded[0] >> 3) & 0x3);
         }
         public AccelConfigRegister (AccelConfigRegister other)
            : this(other.Encode())
         {
         }
         public Byte[] Encode ()
         {
            return new Byte[1]
            {
               (Byte)(
                  (this.SelfTestX ? 1 << 7 : 0 ) | 
                  (this.SelfTestY ? 1 << 6 : 0 ) | 
                  (this.SelfTestZ ? 1 << 5 : 0 ) | 
                  ((Byte)this.Scale & 0x3) << 3

               )
            };
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("SelfTestX:     {0}", this.SelfTestX));
            str.AppendLine(String.Format("SelfTestY:     {0}", this.SelfTestY));
            str.AppendLine(String.Format("SelfTestZ:     {0}", this.SelfTestZ));
            str.AppendLine(String.Format("Scale:         {0}", this.Scale));
            return str.ToString();
         }
      }

      public struct PowerConfigRegister
      {
         public Boolean Reset { get; set; }
         public Boolean Sleep { get; set; }
         public Boolean Cycle { get; set; }
         public Boolean DisableTemp { get; set; }
         public ClockSource ClockSource { get; set; }
         public CycleModeFrequency CycleModeFrequency { get; set; }
         public Boolean AccelXStandby { get; set; }
         public Boolean AccelYStandby { get; set; }
         public Boolean AccelZStandby { get; set; }
         public Boolean GyroXStandby { get; set; }
         public Boolean GyroYStandby { get; set; }
         public Boolean GyroZStandby { get; set; }

         public static readonly PowerConfigRegister Default = new PowerConfigRegister()
         {
            Sleep = true,
            ClockSource = ClockSource.Internal8MHz,
            CycleModeFrequency = CycleModeFrequency._1_25Hz
         };

         public PowerConfigRegister (Byte[] encoded) : this()
         {
            this.Sleep = (encoded[0] & (1 << 6)) != 0;
            this.Cycle = (encoded[0] & (1 << 5)) != 0;
            this.DisableTemp = (encoded[0] & (1 << 3)) != 0;
            this.ClockSource = (ClockSource)(encoded[0] & 0x7);
            this.CycleModeFrequency = (CycleModeFrequency)((encoded[1] >> 6) & 0x3);
            this.AccelXStandby = (encoded[1] & (1 << 5)) != 0;
            this.AccelYStandby = (encoded[1] & (1 << 4)) != 0;
            this.AccelZStandby = (encoded[1] & (1 << 3)) != 0;
            this.GyroXStandby = (encoded[1] & (1 << 2)) != 0;
            this.GyroYStandby = (encoded[1] & (1 << 1)) != 0;
            this.GyroZStandby = (encoded[1] & (1 << 0)) != 0;
         }
         public PowerConfigRegister (PowerConfigRegister other)
            : this(other.Encode())
         {
         }
         public Byte[] Encode ()
         {
            return new Byte[2]
            {
               (Byte)(
                  (this.Reset ? 1 << 7 : 0 ) | 
                  (this.Sleep ? 1 << 6 : 0 ) | 
                  (this.Cycle ? 1 << 5 : 0 ) | 
                  (this.DisableTemp ? 1 << 3 : 0 ) | 
                  (Byte)this.ClockSource & 0x7
               ),
               (Byte)(
                  ((Byte)this.CycleModeFrequency & 0x3) << 6 |
                  (this.AccelXStandby ? 1 << 5 : 0 ) | 
                  (this.AccelYStandby ? 1 << 4 : 0 ) | 
                  (this.AccelZStandby ? 1 << 3 : 0 ) | 
                  (this.GyroXStandby ? 1 << 2 : 0 ) | 
                  (this.GyroYStandby ? 1 << 1 : 0 ) | 
                  (this.GyroZStandby ? 1 << 0 : 0 )
               )
            };
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("Reset:         {0}", this.Reset));
            str.AppendLine(String.Format("Sleep:         {0}", this.Sleep));
            str.AppendLine(String.Format("Cycle:         {0}", this.Cycle));
            str.AppendLine(String.Format("DisableTemp:   {0}", this.DisableTemp));
            str.AppendLine(String.Format("ClockSource:   {0}", this.ClockSource));
            str.AppendLine(String.Format("CycleModeFreq: {0}", this.CycleModeFrequency));
            str.AppendLine(String.Format("AccelXStandby: {0}", this.AccelXStandby));
            str.AppendLine(String.Format("AccelYStandby: {0}", this.AccelYStandby));
            str.AppendLine(String.Format("AccelZStandby: {0}", this.AccelZStandby));
            str.AppendLine(String.Format("GyroXStandby:  {0}", this.GyroXStandby));
            str.AppendLine(String.Format("GyroYStandby:  {0}", this.GyroYStandby));
            str.AppendLine(String.Format("GyroZStandby:  {0}", this.GyroZStandby));
            return str.ToString();
         }
      }

      public struct TemperatureRegister
      {
         public Int16 Raw
         { 
            get; private set;
         }
         public Single Celsius
         {
            get { return this.Raw / 340.0f + 35.0f; }
         }
         public Single Fahrenheit
         {
            get { return this.Celsius * 9.0f / 5.0f + 32.0f; }
         }
         public TemperatureRegister (Byte[] encoded) : this()
         {
            this.Raw = (Int16)((UInt16)encoded[0] << 8 | encoded[1]);
         }
         public override String ToString ()
         {
            return String.Format("{0:0.0}C/{1:0.0}F", this.Celsius, this.Fahrenheit);
         }
      }

      public struct SampleRegister
      {
         public Int16 Raw
         {
            get; private set;
         }
         public Single Scale
         {
            get; private set;
         }
         public Single Value
         {
            get { return (Single)this.Raw / this.Scale; }
         }
         public SampleRegister (Byte[] encoded, Single scale) : this()
         {
            this.Raw = (Int16)((UInt16)encoded[0] << 8 | encoded[1]);
            this.Scale = scale;
         }
         public override String ToString ()
         {
            return String.Format("{0:0.0}", this.Value);
         }
      }
   }
}
