using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace NPi
{
   public class Stepper
   {
      public const Int32 MinRpm = -60;
      public const Int32 MaxRpm = 60;
      public const Int32 EpsilonRpm = 5;
      private IStepperDriver driver;
      private Int32 rpm;

      public Stepper (IStepperDriver driver)
      {
         this.driver = driver;
         this.driver.Stop();
      }

      public Int32 StepsPerCycle
      {
         get { return this.driver.StepsPerCycle; }
         set { this.driver.StepsPerCycle = value; }
      }

      public Boolean Reverse
      {
         get; set;
      }

      public Int32 Rpm
      {
         get { return this.rpm; }
         set { this.rpm = Math.Min(Math.Max(value, MinRpm), MaxRpm); }
      }

      public void Stop ()
      {
         this.driver.Stop();
      }

      public void Step (Int32 steps)
      {
         if (this.StepsPerCycle == 0)
            throw new InvalidOperationException("Invalid StepsPerCycle");
         if (Math.Abs(this.rpm) < EpsilonRpm)
            Stop();
         else
            this.driver.Step(steps, !this.Reverse ? this.Rpm : -this.Rpm);
      }

      public void StepReverse (Int32 steps)
      {
         Step(-steps);
      }

      public void Rotate (Double radians)
      {
         Step((Int32)(radians * this.driver.StepsPerCycle / (2 * Math.PI)));
      }

      public void RotateReverse (Double radians)
      {
         StepReverse((Int32)(radians * this.driver.StepsPerCycle / (2 * Math.PI)));
      }

      public void Cycle (Int32 cycles)
      {
         Step(cycles * this.driver.StepsPerCycle);
      }

      public void CycleReverse (Int32 cycles)
      {
         StepReverse(cycles * this.driver.StepsPerCycle);
      }

      public void Run ()
      {
         Step(Int32.MaxValue);
      }

      public void RunReverse ()
      {
         Step(Int32.MinValue);
      }
   }
}
