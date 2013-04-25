using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Text;

namespace NPi
{

   class Lcd
   {
      private Gpio rs;
      private Gpio en;
      private Gpio.Array da;

      private void Main ()
      {
         using (rs = new Gpio(17, Gpio.Mode.Write))
         using (en = new Gpio(18, Gpio.Mode.Write))
         using (da = new Gpio.Array(new[] { 22, 23, 24, 25 }, Gpio.Mode.Write))
         {
            rs.Value = en.Value = false;
            da.Value = 0;
            // startup delay
            Thread.Sleep(50);
            // 4-bit mode
            Write(0x3);
            Thread.Sleep(5);
            // again!
            Write(0x3);
            Thread.Sleep(5);
            // again!
            Write(0x3);
            Thread.Sleep(5);
            // finish init
            Write(0x2);
            // function set, 4 bits, 2 line, 5x8 dots
            Command(0x20 | 0x08);
            // turn on, no cursor, no blink
            Command(0x08 | 0x04);
            // clear output (takes a long time)
            Command(0x01);
            Thread.Sleep(5);
            // set entry mode left-right
            Command(0x04 | 0x02);
            // create a custom character (heart)
            Command(0x40 | 0);
            foreach (var ch in new Byte[] { 0x0, 0xa, 0x1f, 0x1f, 0x1f, 0xe, 0x4, 0x0 })
               Text(ch);
            Command(0x80 | 0x00);   // reset buffer position
            // write a message w/custom character
            Text("Brent \0 Kelly");
            // newline (setcursor row 2)
            Command(0x80 | 0x40);
            // write a message
            Text("Tito \0 Food");
         }
      }

      private void Command (Byte value)
      {
         rs.Value = false;
         Write(value);
      }
      private void Text (Byte value)
      {
         rs.Value = true;
         Write(value);
      }
      private void Text (String str)
      {
         foreach (var b in Encoding.ASCII.GetBytes(str))
            Text(b);
      }
      private void Write (Byte value)
      {
         da.Value = (value >> 4);
         PulseEnable();
         da.Value = value & 0x0F;
         PulseEnable();
      }
      private void PulseEnable ()
      {
         en.Value = false;
         Thread.Sleep(1);
         en.Value = true;
         Thread.Sleep(1);
         en.Value = false;
         Thread.Sleep(1);
      }
   }
}
