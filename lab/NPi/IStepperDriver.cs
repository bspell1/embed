using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NPi
{
   public interface IStepperDriver
   {
      Int32 StepsPerCycle { get; set; }
      void Stop ();
      void Step (Int32 steps, Int32 rpm);
   }
}
