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

      public const Int32 MaxPayload = 32;
      private const Byte CommandRegisterRead = 0x00;
      private const Byte CommandRegisterWrite = 0x20;
      private const Byte CommandRXReadPacket = 0x61;
      private const Byte CommandTXWritePacket = 0xA0;
      private const Byte CommandTXFlush = 0xE1;
      private const Byte CommandRXFlush = 0xE2;
      private const Byte CommandTXReuse = 0xE3;
      private const Byte CommandActivate = 0x50;
      private const Byte CommandRXReadDynamicLength = 0x60;
      private const Byte CommandTXWriteNoAck = 0xB0;
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
      private Gpio irqPin;
      private Byte[] buffer;
      private ConfigRegister config;
      private FeatureRegister features;

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
            this.cePin = new Gpio(cePin, Gpio.Mode.Output);
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

      public Nrf24 (String path, Int32 cePin, Int32 irqPin, Reactor reactor)
         : this(path, cePin)
      {
         if (irqPin == 0)
            reactor.Poll(
               () => this.Status.Interrupts != Interrupt.None,
               this.OnInterrupt
            );
         else
         {
            this.irqPin = new Gpio(irqPin, Gpio.TriggerEdge.Falling, reactor);
            this.irqPin.Triggered += this.OnInterrupt;
         }
      }

      public Nrf24 (String path, Int32 cePin, Reactor reactor)
         : this(path, cePin, 0, reactor)
      {
      }

      public void Dispose ()
      {
         if (this.spi != null)
            this.spi.Dispose();
         this.spi = null;
         if (this.cePin != null)
            this.cePin.Dispose();
         this.cePin = null;
         if (this.irqPin != null)
            this.irqPin.Dispose();
         this.irqPin = null;
      }

      public event Action<StatusRegister> IrqTriggered;
      public event Action<StatusRegister> RXDataReady;
      public event Action<StatusRegister> TXDataSent;
      public event Action<StatusRegister> TXRetryFailed;

      #region Registers
      public ConfigRegister Config
      {
         get
         {
            return this.config;
         }
         set
         {
            WriteRegister(RegAddressConfig, value.Encode());
            this.config = value;
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
         get { return GetRXAddress(0); }
         set { SetRXAddress(0, value); }
      }
      public String RXAddress1
      {
         get { return GetRXAddress(1); }
         set { SetRXAddress(1, value); }
      }
      public String RXAddress2
      {
         get { return GetRXAddress(2); }
         set { SetRXAddress(2, value); }
      }
      public String RXAddress3
      {
         get { return GetRXAddress(3); }
         set { SetRXAddress(3, value); }
      }
      public String RXAddress4
      {
         get { return GetRXAddress(4); }
         set { SetRXAddress(4, value); }
      }
      public String RXAddress5
      {
         get { return GetRXAddress(5); }
         set { SetRXAddress(5, value); }
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
         get { return GetRXLength(0); }
         set { SetRXLength(0, value); }
      }
      public Int32 RXLength1
      {
         get { return GetRXLength(1); }
         set { SetRXLength(1, value); }
      }
      public Int32 RXLength2
      {
         get { return GetRXLength(2); }
         set { SetRXLength(2, value); }
      }
      public Int32 RXLength3
      {
         get { return GetRXLength(3); }
         set { SetRXLength(3, value); }
      }
      public Int32 RXLength4
      {
         get { return GetRXLength(4); }
         set { SetRXLength(4, value); }
      }
      public Int32 RXLength5
      {
         get { return GetRXLength(5); }
         set { SetRXLength(5, value); }
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
            return this.features;
         }
         set
         {
            WriteRegister(RegAddressFeature, value.Encode());
            this.features = value;
         }
      }
      #endregion

      #region Operations
      private void OnInterrupt ()
      {
         var status = ClearInterrupts();
         if (this.IrqTriggered != null)
            this.IrqTriggered(status);
         if (status.Interrupts.HasFlag(Interrupt.RXDataReady))
            if (this.RXDataReady != null)
               this.RXDataReady(status);
         if (status.Interrupts.HasFlag(Interrupt.TXDataSent))
            if (this.TXDataSent != null)
               this.TXDataSent(status);
         if (status.Interrupts.HasFlag(Interrupt.TXRetryFailed))
            if (this.TXRetryFailed != null)
               this.TXRetryFailed(status);
      }
      public String GetRXAddress (Int32 pipe)
      {
         if (pipe < 0 || pipe > 5)
            throw new ArgumentOutOfRangeException("pipe");
         var address = "";
         if (pipe == 0)
         {
            ReadRegister(RegAddressRXAddress0, 5);
            address = Encoding.ASCII.GetString(
               this.buffer.Skip(1).Take(5).Reverse().ToArray()
            );
         }
         else
         {
            ReadRegister(RegAddressRXAddress1, 5);
            address = Encoding.ASCII.GetString(
               this.buffer.Skip(1).Take(5).Reverse().ToArray()
            );
            if (pipe > 1)
            {
               address = address.Substring(0, address.Length - 1);
               ReadRegister(RegAddressRXAddress0 + pipe, 1);
               address += Encoding.ASCII.GetChars(this.buffer, 1, 1)[0];
            }
         }
         return address;
      }
      public void SetRXAddress (Int32 pipe, String address)
      {
         if (pipe < 0 || pipe > 5)
            throw new ArgumentOutOfRangeException("pipe");
         if (address == null || address.Length < 3 || address.Length > 5)
            throw new ArgumentException("address");
         if (pipe == 0)
            WriteRegister(
               RegAddressRXAddress0,
               Encoding.ASCII.GetBytes(address).Reverse().ToArray()
            );
         else if (pipe == 1)
            WriteRegister(
               RegAddressRXAddress1,
               Encoding.ASCII.GetBytes(address).Reverse().ToArray()
            );
         else
            WriteRegister(
               RegAddressRXAddress0 + pipe,
               Encoding.ASCII.GetBytes(new[] { address.Last() })
            );
      }
      public Int32 GetRXLength (Int32 pipe)
      {
         if (pipe < 0 || pipe > 5)
            throw new ArgumentOutOfRangeException("pipe");
         ReadRegister(RegAddressRXLength0 + pipe, 1);
         return (Int32)this.buffer[1];
      }
      public void SetRXLength (Int32 pipe, Int32 length)
      {
         if (pipe < 0 || pipe > 5)
            throw new ArgumentOutOfRangeException("pipe");
         if (length < 0 || length > MaxPayload)
            throw new ArgumentOutOfRangeException("length");
         WriteRegister(RegAddressRXLength0 + pipe, (Byte)length);
      }
      public Int32 GetRXDynamicLength ()
      {
         this.buffer[0] = CommandRXReadDynamicLength;
         ReadWrite(1);
         return this.buffer[1];
      }
      public StatusRegister ClearInterrupts (Interrupt interrupt = Interrupt.All)
      {
         WriteRegister(RegAddressStatus, (Byte)((Byte)interrupt << 4));
         return new StatusRegister(this.buffer[0]);
      }
      public void TransmitPacket (Byte[] data)
      {
         if (this.config.Mode != Mode.Transmit)
            throw new InvalidOperationException("Tranceiver not configured for transmit");
         this.buffer[0] = (this.Features.DisableAck) ? 
            CommandTXWriteNoAck : 
            CommandTXWritePacket;
         Array.Copy(data, 0, this.buffer, 1, data.Length);
         ReadWrite(data.Length);
         this.cePin.Value = true;
         this.cePin.Value = false;
      }
      public void FlushTransmit ()
      {
         this.buffer[0] = CommandTXFlush;
         ReadWrite(0);
      }
      public void Listen ()
      {
         if (this.config.Mode != Mode.Receive)
            throw new InvalidOperationException("The tranceiver is not configured for receive");
         this.cePin.Value = true;
      }
      public void Unlisten ()
      {
         if (this.config.Mode != Mode.Receive)
            throw new InvalidOperationException("The tranceiver is not configured for receive");
         this.cePin.Value = false;
      }
      public Byte[] ReceivePacket (Byte[] buffer, Int32 length)
      {
         if (this.config.Mode != Mode.Receive)
            throw new InvalidOperationException("The tranceiver is not configured for receive");
         this.buffer[0] = CommandRXReadPacket;
         ReadWrite(length);
         Array.Copy(this.buffer, 1, buffer, 0, length);
         return buffer;
      }
      public Byte[] ReceivePacket (Byte[] buffer)
      {
         return ReceivePacket(buffer, buffer.Length);
      }
      public Byte[] ReceivePacket (Int32 pipe)
      {
         var length = GetRXLength(pipe);
         return ReceivePacket(new Byte[length]);
      }
      public void FlushReceive ()
      {
         this.buffer[0] = CommandRXFlush;
         ReadWrite(0);
      }
      public void Validate ()
      {
         var addrLength = this.AddressWidth;
         var rxEnabled = this.RXEnabled;
         switch (this.config.Mode)
         {
            case Mode.Transmit:
               if (this.TXAddress.Length != addrLength)
                  throw new InvalidOperationException("Invalid TX address length");
               if (this.Features.DisableAck && this.AutoAck[0])
                  throw new InvalidOperationException("Auto-ack must be disabled on pipe 0 if the disable ack feature is set");
               break;
            case Mode.Receive:
               var addrs = new[]
               { 
                  this.RXAddress0, 
                  this.RXAddress1,
                  this.RXAddress2,
                  this.RXAddress3,
                  this.RXAddress4,
                  this.RXAddress5
               };
               if (rxEnabled[0] && addrs[1].Length != addrLength)
                  throw new InvalidOperationException("Invalid RX0 address length");
               if (rxEnabled[1] && addrs[2].Length != addrLength)
                  throw new InvalidOperationException("Invalid RX1 address length");
               // ensure that receive address LSB are unique for all addresses,
               // including receive address 0 and 1 (they are not that independent)
               for (var i = 0; i < addrs.Length; i++)
                  for (var j = i + 1; j < addrs.Length; j++)
                     if (rxEnabled[i] && rxEnabled[j] && addrs[i].Last() == addrs[j].Last())
                        throw new InvalidOperationException(
                           String.Format("Duplicate active RX address LSB: {0} {1}", addrs[i], addrs[j])
                        );
               break;
            default:
               throw new InvalidOperationException("Invalid TX/RX mode");
         }
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
         public static readonly PipeFlagRegister None =
            new PipeFlagRegister(0);
         public static readonly PipeFlagRegister All =
            new PipeFlagRegister(0x3F);
         public static readonly PipeFlagRegister AutoAckDefault = 
            All;
         public static readonly PipeFlagRegister RXEnabledDefault =
            new PipeFlagRegister(0x03);
         public static readonly PipeFlagRegister DynPayloadDefault = 
            None;

         private Byte data;

         public PipeFlagRegister (PipeFlagRegister other)
            : this(other.Encode())
         {
         }
         public PipeFlagRegister (Byte data)
         {
            this.data = (Byte)(data & 0x3F);
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
         public Boolean HasAny
         {
            get { return this.data != 0; }
         }
         public Boolean HasAll
         {
            get { return this.data == 0x3F; }
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

         public static PipeFlagRegister operator ~ (PipeFlagRegister r)
         {
            return new PipeFlagRegister((Byte)~r.data);
         }
         public static PipeFlagRegister operator & (PipeFlagRegister r1, PipeFlagRegister r2)
         {
            return new PipeFlagRegister((Byte)(r1.data & r2.data));
         }
         public static PipeFlagRegister operator | (PipeFlagRegister r1, PipeFlagRegister r2)
         {
            return new PipeFlagRegister((Byte)(r1.data | r2.data));
         }
         public static PipeFlagRegister operator ^ (PipeFlagRegister r1, PipeFlagRegister r2)
         {
            return new PipeFlagRegister((Byte)(r1.data ^ r2.data));
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
