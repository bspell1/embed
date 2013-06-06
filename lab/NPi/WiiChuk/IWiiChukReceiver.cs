using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.WiiChuk
{
   public interface IWiiChukReceiver
   {
      Int32 Count { get; set; }
      event Action<Byte[]> Received;
   }
}
