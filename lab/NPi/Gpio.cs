﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace NPi
{
   public class Gpio : IDisposable
   {
      public enum Mode
      {
         Read,
         Write
      }

      private const String RootPath = "/sys/class/gpio";
      private Int32 pin;
      private Stream stream;

      public Gpio (Int32 pin, Mode mode)
      {
         this.pin = pin;
         try
         {
            var expPath = Path.Combine(RootPath, "export");
            using (var exp = new FileStream(expPath, FileMode.Open, FileAccess.Write))
            using (var writer = new StreamWriter(exp, Encoding.ASCII))
               writer.Write("{0}", pin);
            var pinPath = Path.Combine(RootPath, String.Format("gpio{0}", pin));
            var dirPath = Path.Combine(pinPath, "direction");
            using (var dir = new FileStream(dirPath, FileMode.Open, FileAccess.Write))
            using (var writer = new StreamWriter(dir, Encoding.ASCII))
               writer.Write(mode == Mode.Write ? "out" : "in");
            var valPath = Path.Combine(pinPath, "value");
            this.stream = new FileStream(
               valPath,
               FileMode.Open,
               mode == Mode.Write ? FileAccess.Write : FileAccess.Read,
               FileShare.Read,
               1,
               FileOptions.WriteThrough
            );
         }
         catch
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.stream != null)
         {
            try { this.Value = false; }
            catch { }
            try { this.stream.Dispose(); }
            catch { }
         }
         this.stream = null;
         if (this.pin != 0)
         {
            try
            {
               var unexpPath = Path.Combine(RootPath, "unexport");
               using (var unexp = new FileStream(unexpPath, FileMode.Open, FileAccess.Write, FileShare.ReadWrite))
               using (var writer = new StreamWriter(unexp, Encoding.ASCII))
                  writer.Write("{0}", this.pin);
            }
            catch { }
         }
         this.pin = 0;
      }

      public Boolean Value
      {
         get
         {
            var buffer = new Byte[1];
            this.stream.Read(buffer, 0, 1);
            return Encoding.ASCII.GetString(buffer) == "1";
         }
         set
         {
            this.stream.Write(Encoding.ASCII.GetBytes(value ? "1" : "0"), 0, 1);
            this.stream.Flush();
         }
      }

      public class Array : IDisposable
      {
         private Gpio[] pins;

         public Array (Int32[] pins, Mode mode)
         {
            this.pins = new Gpio[pins.Length];
            try
            {
               for (var i = 0; i < pins.Length; i++)
                  this.pins[i] = new Gpio(pins[i], mode);
            }
            catch
            {
               Dispose();
               throw;
            }
         }
         public void Dispose ()
         {
            if (this.pins != null)
            {
               for (var i = 0; i < this.pins.Length; i++)
               {
                  if (this.pins[i] != null)
                     this.pins[i].Dispose();
                  this.pins[i] = null;
               }
            }
            this.pins = null;
         }

         public Boolean this[Int32 pin]
         {
            get
            {
               return this.pins[pin].Value;
            }
            set
            {
               this.pins[pin].Value = value;
            }
         }
         public Int32 Value
         {
            get
            {
               var value = 0;
               for (var i = this.pins.Length - 1; i >= 0; i--)
                  value = (value << 1) & (this.pins[i].Value ? 1 : 0);
               return value;
            }
            set
            {
               for (var i = 0; i < this.pins.Length; i++)
                  this.pins[i].Value = ((value >> i) & 1) == 1;
            }
         }
      }
   }
}
