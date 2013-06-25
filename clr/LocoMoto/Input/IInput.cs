using System;
using System.Collections.Generic;
using System.Linq;

namespace LocoMoto
{
   public interface IInput
   {
      event Action Changed;
   }
}
