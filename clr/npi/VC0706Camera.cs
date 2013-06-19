using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;

namespace NPi
{
   public class VC0706Camera : IDisposable
   {
      private const Byte RequestMagic = 0x56;
      private const Byte ResponseMagic = 0x76;
      private const Byte ResponseErrorOK = 0x00;
      private const Byte ResponseErrorNotReceived = 0x01;
      private const Byte ResponseErrorDataLength = 0x02;
      private const Byte CommandGetVersion = 0x11;
      private const Byte CommandReset = 0x26;
      private const Byte CommandSetSerialNumber = 0x21;
      private const Byte CommandSetPort = 0x24;
      private const Byte CommandFrameControl = 0x36;
      private const Byte CommandFrameGetLength = 0x34;
      private const Byte CommandFrameRead = 0x32;
      private const Byte CommandFrameControlStopCurrent = 0x00;
      private const Byte CommandFrameControlStopNext = 0x01;
      private const Byte CommandFrameControlResume = 0x02;
      private const Byte CommandFrameControlStep = 0x03;
      private const Byte FrameCurrent = 0x00;
      private const Byte FrameNext = 0x01;
      private const Byte ReadFrameModeDma = 0x0A;
      private static readonly Byte[] PortBaud38400 = { 0x2A, 0xF2 };
      private static readonly Byte[] PortBaud115200 = { 0x0D, 0xA6 };
      private static readonly Byte[] EmptyData = { };
      private SerialPort port;
      private Byte serialNumber;

      public VC0706Camera (String path)
      {
         this.port = new SerialPort(path, 38400, Parity.None, 8, StopBits.One);
         this.port.ReadTimeout = 2000;
         this.port.ReadBufferSize = 65536;
         try
         {
            this.port.Open();
            // upgrade to 115200 baud
            try
            {
               Execute(CommandSetPort, 0x01, PortBaud115200[0], PortBaud115200[1]);
               this.port.BaudRate = 115200;
            }
            catch
            {
               this.port.BaudRate = 115200;
               Execute(CommandSetPort, 0x01, PortBaud115200[0], PortBaud115200[1]);
            }
         }
         catch
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.port != null)
         {
            // reset to 38400 baud
            try { Execute(CommandSetPort, 0x01, PortBaud38400[0], PortBaud38400[1]); }
            catch { }
            this.port.Dispose();
         }
         this.port = null;
      }

      public String Version
      {
         get
         {
            return Encoding.ASCII.GetString(Execute(CommandGetVersion));
         }
      }

      public Int32 SerialNumber
      {
         get { return this.serialNumber; }
         set
         {
            if (value > Byte.MaxValue)
               throw new ArgumentOutOfRangeException("value");
            Execute(CommandSetSerialNumber, (Byte)value);
            this.serialNumber = (Byte)value;
         }
      }

      public void Reset ()
      {
         Execute(CommandReset);
         System.Threading.Thread.Sleep(10);
      }

      public Int32 BeginCaptureImage ()
      {
         // stop the next frame
         Execute(CommandFrameControl, CommandFrameControlStopNext);
         // retrieve the frame length
         var lenData = Execute(CommandFrameGetLength, FrameNext);
         var frameLen = (Int32)
            (lenData[0] << 24) |
            (lenData[1] << 16) |
            (lenData[2] << 8) |
            (lenData[3] << 0);
         // execute the read command
         Execute(
            CommandFrameRead,
            FrameNext,
            ReadFrameModeDma,
            0, 0, 0, 0,                                     // offset 0,
            lenData[0], lenData[1], lenData[2], lenData[3], // whole buffer
            0, 10                                           // delay 0.1ms
         );
         return frameLen;
      }
      public void EndCaptureImage (Byte[] frameData, Int32 frameLen)
      {
         try
         {
            // read the results
            var frameRead = 0;
            do
               frameRead += this.port.Read(frameData, frameRead, frameLen - frameRead);
            while (frameRead < frameLen);
            // read the repeated response header
            Response.Read(this.port);
         }
         finally
         {
            // resume the next frame
            Execute(CommandFrameControl, CommandFrameControlResume);
         }
      }
      public Byte[] CaptureImage ()
      {
         var frameLen = BeginCaptureImage();
         var frameData = new Byte[frameLen];
         EndCaptureImage(frameData, frameLen);
         return frameData;
      }

      private Byte[] Execute (Byte command, params Byte[] data)
      {
         this.port.DiscardInBuffer();
         new Request()
         {
            SerialNumber = this.serialNumber,
            Command = command,
            Data = data
         }.Write(this.port);
         var response = Response.Read(this.port);
         if (response.Command != command)
            throw new InvalidDataException();
         if (response.SerialNumber != this.serialNumber)
            throw new InvalidDataException();
         return response.Data;
      }

      private struct Request
      {
         public Byte SerialNumber;
         public Byte Command;
         public Byte[] Data;

         public void Write (SerialPort port)
         {
            var data = this.Data ?? EmptyData;
            port.Write(
               new Byte[]
               {
                  RequestMagic, 
                  this.SerialNumber, 
                  this.Command, 
                  (Byte)data.Length
               }, 
               0, 
               4
            );
            if (data.Length > 0)
               port.Write(data, 0, data.Length);
         }
      }

      private struct Response
      {
         public Byte SerialNumber;
         public Byte Command;
         public Byte[] Data;

         public static Response Read (SerialPort port)
         {
            var headerData = new Byte[5];
            var headerRead = 0;
            do
               headerRead += port.Read(headerData, headerRead, headerData.Length - headerRead);
            while (headerRead < headerData.Length);
            if (headerData[0] != ResponseMagic)
               throw new InvalidDataException();
            if (headerData[3] != ResponseErrorOK)
               throw new InvalidOperationException(
                  String.Format(
                     "Protocol error: {0:X} for command {1:X}", 
                     headerData[3], 
                     headerData[2]
                  )
               );
            var data = (headerData[4] > 0) ? new Byte[headerData[4]] : EmptyData;
            if (headerData[4] > 0)
            {
               var dataRead = 0;
               do
                  dataRead += port.Read(data, dataRead, data.Length - dataRead);
               while (dataRead < data.Length);
            }
            return new Response()
            {
               SerialNumber = headerData[1],
               Command = headerData[2],
               Data = data
            };
         }
      }
   }
}
