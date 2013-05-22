using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace NPi
{
   public class Nrf24 : IDisposable
   {
      [Flags]
      public enum Interrupt
      {
         None = 0x00,
         RXDataReady = 0x04,
         TXDataSent = 0x02,
         TXRetryFailed = 0x01,
         All = RXDataReady | TXDataSent | TXRetryFailed
      }

      [Flags]
      public enum Mode
      {
         Off = 0x0,
         Transmit = 0x2,
         Receive = 0x3
      }

      public enum Crc
      {
         Default = OneByte,
         None = 0x0,
         OneByte = 0x2,
         TwoByte = 0x3
      }

      public enum BitRate
      {
         Default = TwoMbps,
         OneMbps = 0x0,
         TwoMbps = 0x1
      }

      public enum RFPower
      {
         Default = Minus0dBm,
         Minus18dBm = 0x0,
         Minus12dBm = 0x1,
         Minus6dBm = 0x2,
         Minus0dBm = 0x3
      }

      private const Byte CommandRegisterRead = 0x00;
      private const Byte CommandRegisterWrite = 0x20;
      private const Byte CommandRXReadPacket = 0x61;
      private const Byte CommandTXWritePacket = 0xA0;
      private const Byte CommandTXFlush = 0xE1;
      private const Byte CommandRXFlush = 0xE2;
      private const Byte CommandTXReuse = 0xE3;
      private const Byte CommandActivate = 0x50;
      private const Byte CommandTXWriteNoAck= 0xB0;
      private const Byte CommandNoop = 0xFF;
      private const Byte RegAddressConfig = 0x00;
      private const Byte RegAddressAutoAck = 0x01;
      private const Byte RegAddressRXEnabled = 0x02;
      private const Byte RegAddressAddressWidth = 0x03;
      private const Byte RegAddressAutoRetry = 0x04;
      private const Byte RegAddressRFChannel = 0x05;
      private const Byte RegAddressRFConfig = 0x06;
      private const Byte RegAddressStatus = 0x07;
      private const Byte RegAddressTXStats = 0x08;
      private const Byte RegAddressCarrierDetect = 0x09;
      private const Byte RegAddressRXAddress0 = 0x0A;
      private const Byte RegAddressRXAddress1 = 0x0B;
      private const Byte RegAddressRXAddress2 = 0x0C;
      private const Byte RegAddressRXAddress3 = 0x0D;
      private const Byte RegAddressRXAddress4 = 0x0E;
      private const Byte RegAddressRXAddress5 = 0x0F;
      private const Byte RegAddressTXAddress = 0x10;
      private const Byte RegAddressRXLength0 = 0x11;
      private const Byte RegAddressRXLength1 = 0x12;
      private const Byte RegAddressRXLength2 = 0x13;
      private const Byte RegAddressRXLength3 = 0x14;
      private const Byte RegAddressRXLength4 = 0x15;
      private const Byte RegAddressRXLength5 = 0x16;
      private const Byte RegAddressFifoStatus = 0x17;
      private const Byte RegAddressDynPayload = 0x1C;
      private const Byte RegAddressFeature = 0x1D;

      private SpiDevice spi;
      private Gpio cePin;
      private Byte[] buffer;

      public Nrf24 (String path, Int32 cePin)
      {
         try
         {
            this.spi = new SpiDevice(path)
            {
               Mode = SpiMode.Mode0,
               BitsPerWord = 8,
               ClockSpeed = 8000000,
               LsbFirst = false
            };
            this.cePin = new Gpio(cePin, Gpio.Mode.Write);
            this.buffer = new Byte[33];
            // initialize register defaults
            this.cePin.Value = false;
            this.Config = ConfigRegister.Default;
            this.AutoAck = PipeFlagRegister.AutoAckDefault;
            this.RXEnabled = PipeFlagRegister.RXEnabledDefault;
            this.AddressWidth = 5;
            this.AutoRetry = AutoRetryRegister.Default;
            this.RFChannel = 2;
            this.RFConfig = RFConfigRegister.Default;
            this.RXAddress0 = "nrf00";
            this.RXAddress1 = "nrf01";
            this.RXAddress2 = "nrf02";
            this.RXAddress3 = "nrf03";
            this.RXAddress4 = "nrf04";
            this.RXAddress5 = "nrf05";
            this.TXAddress = "nrf00";
            this.RXLength0 = 0;
            this.RXLength1 = 0;
            this.RXLength2 = 0;
            this.RXLength3 = 0;
            this.RXLength4 = 0;
            this.RXLength5 = 0;
            this.DynPayload = PipeFlagRegister.DynPayloadDefault;
            this.Features = FeatureRegister.Default;
            ClearInterrupts();
            FlushTransmit();
            FlushReceive();
         }
         catch
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.spi != null)
            this.spi.Dispose();
         this.spi = null;
         if (this.cePin != null)
            this.cePin.Dispose();
         this.cePin = null;
      }

      #region Registers
      public ConfigRegister Config
      {
         get
         {
            ReadRegister(RegAddressConfig, 1);
            return new ConfigRegister(this.buffer[1]);
         }
         set
         {
            WriteRegister(RegAddressConfig, value.Encode());
         }
      }

      public PipeFlagRegister AutoAck
      {
         get
         {
            ReadRegister(RegAddressAutoAck, 1);
            return new PipeFlagRegister(this.buffer[1]);
         }
         set
         {
            WriteRegister(RegAddressAutoAck, value.Encode());
         }
      }

      public PipeFlagRegister RXEnabled
      {
         get
         {
            ReadRegister(RegAddressRXEnabled, 1);
            return new PipeFlagRegister(this.buffer[1]);
         }
         set
         {
            WriteRegister(RegAddressRXEnabled, value.Encode());
         }
      }
      public Int32 AddressWidth
      {
         get
         {
            ReadRegister(RegAddressAddressWidth, 1);
            return this.buffer[1] + 2;
         }
         set
         {
            if (value < 3 || value > 5)
               throw new ArgumentOutOfRangeException("AddressWidth");
            WriteRegister(RegAddressAddressWidth, (Byte)(value - 2));
         }
      }
      public AutoRetryRegister AutoRetry
      {
         get
         {
            ReadRegister(RegAddressAutoRetry, 1);
            return new AutoRetryRegister(this.buffer[1]);
         }
         set
         {
            if (value.Delay <= 0 || value.Delay > 4000 || value.Delay % 250 != 0)
               throw new ArgumentOutOfRangeException("Delay");
            if (value.Count < 0 || value.Count > 15)
               throw new ArgumentOutOfRangeException("Count");
            WriteRegister(RegAddressAutoRetry, value.Encode());
         }
      }
      public Int32 RFChannel
      {
         get
         {
            ReadRegister(RegAddressRFChannel, 1);
            return this.buffer[1];
         }
         set
         {
            if (value < 0 || value > 83)
               throw new ArgumentOutOfRangeException("RFChannel");
            WriteRegister(RegAddressRFChannel, (Byte)value);
         }
      }
      public RFConfigRegister RFConfig
      {
         get
         {
            ReadRegister(RegAddressRFConfig, 1);
            return new RFConfigRegister(this.buffer[1]);
         }
         set
         {
            WriteRegister(RegAddressRFConfig, value.Encode());
         }
      }
      public StatusRegister Status
      {
         get
         {
            this.buffer[0] = CommandNoop;
            ReadWrite(0);
            return new StatusRegister(this.buffer[0]);
         }
      }
      public TXStatsRegister TXStats
      {
         get
         {
            ReadRegister(RegAddressTXStats, 1);
            return new TXStatsRegister(this.buffer[1]);
         }
      }
      public Boolean CarrierDetect
      {
         get
         {
            ReadRegister(RegAddressCarrierDetect, 1);
            return (this.buffer[1] & 0x1) != 0;
         }
      }
      public String RXAddress0
      {
         get
         {
            ReadRegister(RegAddressRXAddress0, 5);
            return Encoding.ASCII.GetString(
               this.buffer.Skip(1).Take(5).Reverse().ToArray()
            );
         }
         set
         {
            if (value == null || value.Length < 3 || value.Length > 5)
               throw new ArgumentException("RXAddress0");
            WriteRegister(
               RegAddressRXAddress0, 
               Encoding.ASCII.GetBytes(value).Reverse().ToArray()
            );
         }
      }
      public String RXAddress1
      {
         get
         {
            ReadRegister(RegAddressRXAddress1, 5);
            return Encoding.ASCII.GetString(
               this.buffer.Skip(1).Take(5).Reverse().ToArray()
            );
         }
         set
         {
            if (value == null || value.Length < 3 || value.Length > 5)
               throw new ArgumentException("RXAddress1");
            WriteRegister(
               RegAddressRXAddress1,
               Encoding.ASCII.GetBytes(value).Reverse().ToArray()
            );
         }
      }
      public String RXAddress2
      {
         get
         {
            var prefix = this.RXAddress1;
            prefix = prefix.Substring(0, prefix.Length - 1);
            ReadRegister(RegAddressRXAddress2, 1);
            return prefix + Encoding.ASCII.GetChars(this.buffer, 1, 1)[0];
         }
         set
         {
            WriteRegister(
               RegAddressRXAddress2, 
               Encoding.ASCII.GetBytes(new[] { value.Last() })
            );
         }
      }
      public String RXAddress3
      {
         get
         {
            var prefix = this.RXAddress1;
            prefix = prefix.Substring(0, prefix.Length - 1);
            ReadRegister(RegAddressRXAddress3, 1);
            return prefix + Encoding.ASCII.GetChars(this.buffer, 1, 1)[0];
         }
         set
         {
            WriteRegister(
               RegAddressRXAddress3,
               Encoding.ASCII.GetBytes(new[] { value.Last() })
            );
         }
      }
      public String RXAddress4
      {
         get
         {
            var prefix = this.RXAddress1;
            prefix = prefix.Substring(0, prefix.Length - 1);
            ReadRegister(RegAddressRXAddress4, 1);
            return prefix + Encoding.ASCII.GetChars(this.buffer, 1, 1)[0];
         }
         set
         {
            WriteRegister(
               RegAddressRXAddress4,
               Encoding.ASCII.GetBytes(new[] { value.Last() })
            );
         }
      }
      public String RXAddress5
      {
         get
         {
            var prefix = this.RXAddress1;
            prefix = prefix.Substring(0, prefix.Length - 1);
            ReadRegister(RegAddressRXAddress5, 1);
            return prefix + Encoding.ASCII.GetChars(this.buffer, 1, 1)[0];
         }
         set
         {
            WriteRegister(
               RegAddressRXAddress5,
               Encoding.ASCII.GetBytes(new[] { value.Last() })
            );
         }
      }
      public String TXAddress
      {
         get
         {
            ReadRegister(RegAddressTXAddress, 5);
            return Encoding.ASCII.GetString(
               this.buffer.Skip(1).Take(5).Reverse().ToArray()
            );
         }
         set
         {
            if (value == null || value.Length < 3 || value.Length > 5)
               throw new ArgumentException("TXAddress");
            WriteRegister(
               RegAddressTXAddress,
               Encoding.ASCII.GetBytes(value).Reverse().ToArray()
            );
         }
      }
      public Int32 RXLength0
      {
         get
         {
            ReadRegister(RegAddressRXLength0, 1);
            return this.buffer[1];
         }
         set
         {
            if (value < 0 || value > 32)
               throw new ArgumentOutOfRangeException("RXLength0");
            WriteRegister(RegAddressRXLength0, (Byte)value);
         }
      }
      public Int32 RXLength1
      {
         get
         {
            ReadRegister(RegAddressRXLength1, 1);
            return this.buffer[1];
         }
         set
         {
            if (value < 0 || value > 32)
               throw new ArgumentOutOfRangeException("RXLength1");
            WriteRegister(RegAddressRXLength1, (Byte)value);
         }
      }
      public Int32 RXLength2
      {
         get
         {
            ReadRegister(RegAddressRXLength2, 1);
            return this.buffer[1];
         }
         set
         {
            if (value < 0 || value > 32)
               throw new ArgumentOutOfRangeException("RXLength2");
            WriteRegister(RegAddressRXLength2, (Byte)value);
         }
      }
      public Int32 RXLength3
      {
         get
         {
            ReadRegister(RegAddressRXLength3, 1);
            return this.buffer[1];
         }
         set
         {
            if (value < 0 || value > 32)
               throw new ArgumentOutOfRangeException("RXLength3");
            WriteRegister(RegAddressRXLength3, (Byte)value);
         }
      }
      public Int32 RXLength4
      {
         get
         {
            ReadRegister(RegAddressRXLength4, 1);
            return this.buffer[1];
         }
         set
         {
            if (value < 0 || value > 32)
               throw new ArgumentOutOfRangeException("RXLength4");
            WriteRegister(RegAddressRXLength4, (Byte)value);
         }
      }
      public Int32 RXLength5
      {
         get
         {
            ReadRegister(RegAddressRXLength5, 1);
            return this.buffer[1];
         }
         set
         {
            if (value < 0 || value > 32)
               throw new ArgumentOutOfRangeException("RXLength5");
            WriteRegister(RegAddressRXLength5, (Byte)value);
         }
      }
      public FifoStatusRegister FifoStatus
      {
         get
         {
            ReadRegister(RegAddressFifoStatus, 1);
            return new FifoStatusRegister(this.buffer[1]);
         }
      }
      public PipeFlagRegister DynPayload
      {
         get
         {
            ReadRegister(RegAddressDynPayload, 1);
            return new PipeFlagRegister(this.buffer[1]);
         }
         set
         {
            WriteRegister(RegAddressDynPayload, value.Encode());
         }
      }
      public FeatureRegister Features
      {
         get
         {
            ReadRegister(RegAddressFeature, 1);
            return new FeatureRegister(this.buffer[1]);
         }
         set
         {
            WriteRegister(RegAddressFeature, value.Encode());
         }
      }
      #endregion

      #region Operations
      public void ClearInterrupts (Interrupt interrupt = Interrupt.All)
      {
         WriteRegister(RegAddressStatus, (Byte)((Byte)interrupt << 4));
      }
      public void TransmitPacket (Byte[] data)
      {
         // TODO: validate packet length
         this.buffer[0] = CommandTXWritePacket;
         for (var i = 0; i < data.Length; i++)
            this.buffer[i + 1] = data[i];
         ReadWrite(data.Length);
         this.cePin.Value = true;
         this.cePin.Value = false;
      }
      public void FlushTransmit ()
      {
         this.buffer[0] = CommandTXFlush;
         ReadWrite(0);
      }
      public void BeginReceivePacket ()
      {
         this.cePin.Value = true;
      }
      public Byte[] EndReceivePacket (Int32 length)
      {
         var timeout = 2000;
         for ( ; ; )
         {
            if (this.Status.Interrupts.HasFlag(Interrupt.RXDataReady))
               break;
            if (--timeout == 0)
               throw new TimeoutException();
            Thread.Sleep(1);
         }
         this.cePin.Value = false;
         Byte[] result = new Byte[length];
         this.buffer[0] = CommandRXReadPacket;
         ReadWrite(length);
         Array.Copy(this.buffer, 1, result, 0, length);
         return result;
      }
      public void FlushReceive ()
      {
         this.buffer[0] = CommandRXFlush;
         ReadWrite(0);
      }
      public String DumpRegisters ()
      {
         var separator = new String('-', 78);
         StringBuilder str = new StringBuilder();
         str.AppendLine(separator);
         str.AppendLine("Config");
         str.Append(this.Config.ToString());
         str.AppendLine(separator);
         str.AppendLine("AutoAck");
         str.Append(this.AutoAck.ToString());
         str.AppendLine(separator);
         str.AppendLine("RXEnabled");
         str.Append(this.RXEnabled.ToString());
         str.AppendLine(separator);
         str.AppendLine(String.Format("AddressWidth:  {0}", this.AddressWidth));
         str.AppendLine(separator);
         str.AppendLine("AutoRetry");
         str.Append(this.AutoRetry.ToString());
         str.AppendLine(separator);
         str.AppendLine(String.Format("RFChannel:     {0}", this.RFChannel));
         str.AppendLine(separator);
         str.AppendLine("RFConfig");
         str.Append(this.RFConfig.ToString());
         str.AppendLine(separator);
         str.AppendLine("Status");
         str.Append(this.Status.ToString());
         str.AppendLine(separator);
         str.AppendLine("TXStats");
         str.Append(this.TXStats.ToString());
         str.AppendLine(separator);
         str.AppendLine(String.Format("CarrierDetect: {0}", this.CarrierDetect));
         str.AppendLine(separator);
         str.AppendLine("RXAddress ");
         str.AppendLine(String.Format("Pipe0:         {0}", this.RXAddress0));
         str.AppendLine(String.Format("Pipe1:         {0}", this.RXAddress1));
         str.AppendLine(String.Format("Pipe2:         {0}", this.RXAddress2));
         str.AppendLine(String.Format("Pipe3:         {0}", this.RXAddress3));
         str.AppendLine(String.Format("Pipe4:         {0}", this.RXAddress4));
         str.AppendLine(String.Format("Pipe5:         {0}", this.RXAddress5));
         str.AppendLine(separator);
         str.AppendLine(String.Format("TXAddress:     {0}", this.TXAddress));
         str.AppendLine(separator);
         str.AppendLine("RXLength");
         str.AppendLine(String.Format("Pipe0:         {0}", this.RXLength0));
         str.AppendLine(String.Format("Pipe1:         {0}", this.RXLength1));
         str.AppendLine(String.Format("Pipe2:         {0}", this.RXLength2));
         str.AppendLine(String.Format("Pipe3:         {0}", this.RXLength3));
         str.AppendLine(String.Format("Pipe4:         {0}", this.RXLength4));
         str.AppendLine(String.Format("Pipe5:         {0}", this.RXLength5));
         str.AppendLine(separator);
         str.AppendLine("FifoStatus");
         str.Append(this.FifoStatus.ToString());
         str.AppendLine(separator);
         str.AppendLine("DynPayload");
         str.Append(this.DynPayload.ToString());
         str.AppendLine(separator);
         str.AppendLine("Features");
         str.Append(this.Features.ToString());
         return str.ToString();
      }
      #endregion

      #region Commands
      private void ReadRegister (Int32 address, Int32 length)
      {
         this.buffer[0] = (Byte)(CommandRegisterRead | address);
         ReadWrite(length);
      }
      private void WriteRegister (Int32 address, params Byte[] data)
      {
         this.buffer[0] = (Byte)(CommandRegisterWrite | address);
         for (var i = 0; i < data.Length; i++)
            this.buffer[i + 1] = data[i];
         ReadWrite(data.Length);
      }
      private void ReadWrite (Int32 dataBytes)
      {
         this.spi.ReadWrite(this.buffer, 0, dataBytes + 1);
      }
      #endregion

      #region Register Types
      public struct ConfigRegister
      {
         public Interrupt Interrupts { get; set; }
         public Crc Crc { get; set; }
         public Mode Mode { get; set; }

         public static readonly ConfigRegister Default = new ConfigRegister()
         {
            Interrupts = Interrupt.All,
            Crc = Crc.Default,
            Mode = Mode.Off
         };
         public ConfigRegister (ConfigRegister other)
            : this(other.Encode())
         {
         }
         public ConfigRegister (Byte data) : this()
         {
            this.Interrupts = (Interrupt)(~(data >> 4) & 0x7);
            this.Crc = (Crc)((data >> 2) & 0x3);
            this.Mode = (Nrf24.Mode)((data >> 0) & 0x3);
         }
         public Byte Encode ()
         {
            return (Byte)(
               ((~(Byte)this.Interrupts & 0x7) << 4) |
               ((Byte)this.Crc << 2) |
               ((Byte)this.Mode << 0)
            );

         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("Interrupts:    {0}", this.Interrupts));
            str.AppendLine(String.Format("Crc:           {0}", this.Crc));
            str.AppendLine(String.Format("Mode:          {0}", this.Mode));
            return str.ToString();
         }
      }
      public struct PipeFlagRegister
      {
         private Byte data;

         public static readonly PipeFlagRegister AutoAckDefault =
            new PipeFlagRegister(0x3F);
         public static readonly PipeFlagRegister RXEnabledDefault =
            new PipeFlagRegister(0x03);
         public static readonly PipeFlagRegister DynPayloadDefault =
            new PipeFlagRegister(0x00);
         public PipeFlagRegister (PipeFlagRegister other)
            : this(other.Encode())
         {
         }
         public PipeFlagRegister (Byte data)
         {
            this.data = data;
         }
         public Byte Encode ()
         {
            return this.data;
         }

         public Boolean this[Int32 pipe]
         {
            get
            {
               return (this.data & (1 << pipe)) != 0;
            }
            set
            {
               if (value)
                  this.data |= (Byte)(1 << pipe);
               else
                  this.data &= (Byte)~(1 << pipe);
            }
         }

         public Boolean Pipe0 { get { return this[0]; } set { this[0] = value; } }
         public Boolean Pipe1 { get { return this[1]; } set { this[1] = value; } }
         public Boolean Pipe2 { get { return this[2]; } set { this[2] = value; } }
         public Boolean Pipe3 { get { return this[3]; } set { this[3] = value; } }
         public Boolean Pipe4 { get { return this[4]; } set { this[4] = value; } }
         public Boolean Pipe5 { get { return this[5]; } set { this[5] = value; } }

         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("Pipe0:         {0}", this.Pipe0));
            str.AppendLine(String.Format("Pipe1:         {0}", this.Pipe1));
            str.AppendLine(String.Format("Pipe2:         {0}", this.Pipe2));
            str.AppendLine(String.Format("Pipe3:         {0}", this.Pipe3));
            str.AppendLine(String.Format("Pipe4:         {0}", this.Pipe4));
            str.AppendLine(String.Format("Pipe5:         {0}", this.Pipe5));
            return str.ToString();
         }
      }
      public struct AutoRetryRegister
      {
         public Int32 Delay { get; set; }
         public Int32 Count { get; set; }
         public Boolean Disabled { get; set; }

         public static readonly AutoRetryRegister Default = new AutoRetryRegister()
         {
            Delay = 250,
            Count = 3
         };
         public AutoRetryRegister (AutoRetryRegister other)
            : this(other.Encode())
         {
         }
         public AutoRetryRegister (Byte data)
            : this()
         {
            this.Delay = ((data >> 4) + 1) * 250;
            this.Count = data & 0xF;
            this.Disabled = (this.Count == 0);
         }
         public Byte Encode ()
         {
            return (Byte)(
               ((this.Delay / 250 - 1) << 4) | 
               (!this.Disabled ? this.Count & 0xF : 0)
            );
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("Delay:         {0}", this.Delay));
            str.AppendLine(String.Format("Count:         {0}", this.Count));
            str.AppendLine(String.Format("Disabled:      {0}", this.Disabled));
            return str.ToString();
         }
      }
      public struct RFConfigRegister
      {
         public BitRate BitRate { get; set; }
         public RFPower RFPower { get; set; }
         public Boolean LnaGain { get; set; }

         public static readonly RFConfigRegister Default = new RFConfigRegister()
         {
            BitRate = BitRate.Default,
            RFPower = RFPower.Default,
            LnaGain = true
         };
         public RFConfigRegister (RFConfigRegister other)
            : this(other.Encode())
         {
         }
         public RFConfigRegister (Byte data)
            : this()
         {
            this.BitRate = (BitRate)((data >> 3) & 0x1);
            this.RFPower = (RFPower)((data >> 1) & 0x3);
            this.LnaGain = (data & 0x1) != 0;
         }
         public Byte Encode ()
         {
            return (Byte)(
               ((Byte)this.BitRate << 3) |
               ((Byte)this.RFPower << 1) | 
               (this.LnaGain ? 1 : 0)
            );
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("BitRate:       {0}", this.BitRate));
            str.AppendLine(String.Format("RFPower:       {0}", this.RFPower));
            str.AppendLine(String.Format("LnaGain:       {0}", this.LnaGain));
            return str.ToString();
         }
      }
      public struct StatusRegister
      {
         public Interrupt Interrupts { get; private set; }
         public Int32 RXReadyPipe { get; private set; }
         public Boolean TXFull { get; private set; }

         public StatusRegister (Byte data) : this()
         {
            this.Interrupts = (Interrupt)((data >> 4) & 0x7);
            this.RXReadyPipe = (data & (1 << 6)) != 0 ? (data >> 1) & 0x07 : -1;
            if (this.RXReadyPipe > 5)
               this.RXReadyPipe = -1;
            this.TXFull = (data & (1 << 0)) != 0;
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("Interrupts:    {0}", this.Interrupts));
            str.AppendLine(String.Format("RXReadyPipe:   {0}", this.RXReadyPipe));
            str.AppendLine(String.Format("TXFull:        {0}", this.TXFull));
            return str.ToString();
         }
      }
      public struct TXStatsRegister
      {
         public Int32 LostPackets { get; private set; }
         public Int32 Retransmits { get; private set; }

         public TXStatsRegister (Byte data)
            : this()
         {
            this.LostPackets = data >> 4;
            this.Retransmits = data & 0xF;
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("LostPackets:   {0}", this.LostPackets));
            str.AppendLine(String.Format("Retransmits:   {0}", this.LostPackets));
            return str.ToString();
         }
      }
      public struct FifoStatusRegister
      {
         public Boolean TXReuse { get; private set; }
         public Boolean TXFull { get; private set; }
         public Boolean TXEmpty { get; private set; }
         public Boolean RXFull { get; private set; }
         public Boolean RXEmpty { get; private set; }
         
         public FifoStatusRegister (Byte data)
            : this()
         {
            this.TXReuse = (data & (1 << 6)) != 0;
            this.TXFull = (data & (1 << 5)) != 0;
            this.TXEmpty = (data & (1 << 4)) != 0;
            this.RXFull = (data & (1 << 1)) != 0;
            this.RXEmpty = (data & (1 << 0)) != 0;
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("TXReuse:       {0}", this.TXReuse));
            str.AppendLine(String.Format("TXFull:        {0}", this.TXFull));
            str.AppendLine(String.Format("TXEmpty:       {0}", this.TXEmpty));
            str.AppendLine(String.Format("RXFull:        {0}", this.RXFull));
            str.AppendLine(String.Format("RXEmpty:       {0}", this.RXEmpty));
            return str.ToString();
         }
      }
      public struct FeatureRegister
      {
         public Boolean DynPayload { get; set; }
         public Boolean AckPayload { get; set; }
         public Boolean DisableAck { get; set; }

         public static readonly FeatureRegister Default = new FeatureRegister()
         {
            DynPayload = false,
            AckPayload = false,
            DisableAck = false
         };
         public FeatureRegister (FeatureRegister other)
            : this(other.Encode())
         {
         }
         public FeatureRegister (Byte data)
            : this()
         {
            this.DynPayload = (data & (1 << 2)) != 0;
            this.AckPayload = (data & (1 << 1)) != 0;
            this.DisableAck = (data & (1 << 0)) != 0;
         }
         public Byte Encode ()
         {
            return (Byte)(
               (this.DynPayload ? 1 << 2 : 0 ) |
               (this.AckPayload ? 1 << 1 : 0) | 
               (this.DisableAck ? 1 << 0 : 0)
            );
         }
         public override String ToString ()
         {
            StringBuilder str = new StringBuilder();
            str.AppendLine(String.Format("DynPayload:    {0}", this.DynPayload));
            str.AppendLine(String.Format("AckPayload:    {0}", this.AckPayload));
            str.AppendLine(String.Format("DisableAck:    {0}", this.DisableAck));
            return str.ToString();
         }
      }
      #endregion
   }
}
