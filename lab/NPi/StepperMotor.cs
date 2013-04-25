using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace NPi
{
   public class StepperMotor : IDisposable
   {
      private static readonly Int32[] CcwSteps = new[]
      {
         10,   // 1010, pink/blue
         6,    // 0110, orange/blue
         5,    // 0101, orange/yellow
         9     // 1001, pink/yellow
      };
      private static readonly Int32[] CwSteps = 
         CcwSteps.Reverse().ToArray();
      private Gpio.Array pins;
      private Int32 steps;
      private Int32 delay;

      public StepperMotor (
         Int32 steps,
         Int32 pink, 
         Int32 orange, 
         Int32 blue, 
         Int32 yellow)
      {
         if (steps <= 0)
            throw new ArgumentException("steps");
         if (pink <= 0)
            throw new ArgumentException("pink");
         if (orange <= 0)
            throw new ArgumentException("orange");
         if (blue <= 0)
            throw new ArgumentException("blue");
         if (yellow <= 0)
            throw new ArgumentException("yellow");
         this.pins = new Gpio.Array(
            new[] { pink, orange, blue, yellow }, 
            Gpio.Mode.Write
         );
         this.steps = steps;
         this.delay = 5;
      }

      public void Dispose ()
      {
         if (this.pins != null)
            this.pins.Dispose();
         this.pins = null;
      }

      public Int32 Rpm
      {
         get
         {
            return (Int32)(60000000000 / (this.delay * this.steps));
         }
         set
         {
            this.delay = (Int32)(60000000000 / value / this.steps);
         }
      }

      public void Step (Int32 steps)
      {
         if (steps >= 0)
            DoStep(steps, CcwSteps);
         else
            DoStep(-steps, CwSteps);
      }

      public void StepReverse (Int32 steps)
      {
         if (steps >= 0)
            DoStep(steps, CwSteps);
         else
            DoStep(-steps, CcwSteps);
      }

      public void Rotate (Double radians)
      {
         Step((Int32)(radians * this.steps / (2 * Math.PI)));
      }

      public void RotateReverse (Double radians)
      {
         StepReverse((Int32)(radians * this.steps / (2 * Math.PI)));
      }

      public void Cycle (Int32 cycles)
      {
         Step(cycles * this.steps);
      }

      public void CycleReverse (Int32 cycles)
      {
         StepReverse(cycles * this.steps);
      }

      private void DoStep (Int32 steps, Int32[] schedule)
      {
         var req = new Mono.Unix.Native.Timespec()
         {
            tv_sec = 0,
            tv_nsec = this.delay
         };
         var rem = new Mono.Unix.Native.Timespec();
         for (var i = 0; i < steps / schedule.Length; i++)
         {
            for (var j = 0; j < schedule.Length; j++)
            {
               this.pins.Value = schedule[j];
               Mono.Unix.Native.Syscall.nanosleep(ref req, ref rem);
            }
         }
      }
   }
}
