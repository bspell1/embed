using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi.Psx
{
   public interface IPsxPadReceiver
   {
      event Action<PsxPadState> Received;
   }
}
