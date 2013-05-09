using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace NPi.BitBanger
{
   class StepperDriver : IStepperDriver, IDisposable
   {
      private static readonly Int32[] CcwStages = new[]
      {
         10,   // 1010, pink/blue
         6,    // 0110, orange/blue
         5,    // 0101, orange/yellow
         9     // 1001, pink/yellow
      };
      private static readonly Int32[] CwStages =
         CcwStages.Reverse().ToArray();
      private Gpio.Array pins;
      private CancellationTokenSource canceler;
      private Task task;

      public StepperDriver (
         Int32 stepsPerCycle,
         Int32 pinkPin, 
         Int32 orangePin, 
         Int32 bluePin, 
         Int32 yellowPin)
      {
         if (stepsPerCycle <= 0)
            throw new ArgumentException("stepsPerCycle");
         if (pinkPin <= 0)
            throw new ArgumentException("pinkPin");
         if (orangePin <= 0)
            throw new ArgumentException("orangePin");
         if (bluePin <= 0)
            throw new ArgumentException("bluePin");
         if (yellowPin <= 0)
            throw new ArgumentException("yellowPin");
         this.pins = new Gpio.Array(
            new[] { pinkPin, orangePin, bluePin, yellowPin }, 
            Gpio.Mode.Write
         );
         this.StepsPerCycle = stepsPerCycle;
      }

      public void Dispose ()
      {
         Stop();
         if (this.pins != null)
            this.pins.Dispose();
         this.pins = null;
      }


      #region IStepperDriver Implementation
      public Int32 StepsPerCycle
      {
         get; set; 
      }
      public void Stop ()
      {
         if (this.task != null)
         {
            if (!this.task.IsCompleted)
               this.canceler.Cancel();
            this.task.Wait();
            this.task.Dispose();
            this.task = null;
            this.canceler.Dispose();
            this.canceler = null;
         }
      }
      public void Step (Int32 steps, Int32 rpm)
      {
         Stop();
         this.canceler = new CancellationTokenSource();
         var cancel = this.canceler.Token;
         var stepsPerCycle = this.StepsPerCycle;
         this.task = Task.Factory.StartNew(
            () =>
            {
               var schedule = ((steps >= 0) != (rpm >= 0)) ? CwStages : CcwStages;
               steps = Math.Abs(steps);
               rpm = Math.Abs(rpm);
               var req = new Mono.Unix.Native.Timespec()
               {
                  tv_sec = 0,
                  tv_nsec = (60000000000 / rpm / stepsPerCycle)
               };
               var rem = new Mono.Unix.Native.Timespec();
               for (var i = 0; i < steps || steps == Int32.MaxValue; i++)
               {
                  if (cancel.IsCancellationRequested)
                     break;
                  for (var j = 0; j < schedule.Length; j++)
                  {
                     this.pins.Value = schedule[j];
                     Mono.Unix.Native.Syscall.nanosleep(ref req, ref rem);
                  }
               }
            }
         );
      }
      #endregion
   }
}
