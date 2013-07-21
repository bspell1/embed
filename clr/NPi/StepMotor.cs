using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace NPi
{
   public class StepMotor
   {
      public const Int32 EpsilonRpm = 5;
      private IStepperDriver driver;
      private Int32 rpm;

      public StepMotor (IStepperDriver driver, Int32 maxRpm)
      {
         this.driver = driver;
         this.driver.Stop();
         this.MaxRpm = maxRpm;
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

      public Int32 MaxRpm
      {
         get; private set;
      }
      public Int32 MinRpm
      {
         get { return -this.MaxRpm; }
      }
      public Int32 Rpm
      {
         get { return this.rpm; }
         set { this.rpm = Math.Min(Math.Max(value, this.MinRpm), this.MaxRpm); }
      }

      public TimeSpan Stop ()
      {
         return this.driver.Stop();
      }

      public TimeSpan Step (Int32 steps)
      {
         if (this.StepsPerCycle == 0)
            throw new InvalidOperationException("Invalid StepsPerCycle");
         return Math.Abs(this.rpm) < EpsilonRpm ?
            this.driver.Stop() : 
            this.driver.Step(steps, !this.Reverse ? this.Rpm : -this.Rpm);
      }

      public TimeSpan StepReverse (Int32 steps)
      {
         return Step(-steps);
      }

      public TimeSpan Rotate (Double radians)
      {
         return Step((Int32)(radians * this.driver.StepsPerCycle / (2 * Math.PI)));
      }

      public TimeSpan RotateReverse (Double radians)
      {
         return StepReverse((Int32)(radians * this.driver.StepsPerCycle / (2 * Math.PI)));
      }

      public TimeSpan Cycle (Int32 cycles)
      {
         return Step(cycles * this.driver.StepsPerCycle);
      }

      public TimeSpan CycleReverse (Int32 cycles)
      {
         return StepReverse(cycles * this.driver.StepsPerCycle);
      }

      public TimeSpan Run ()
      {
         return Step(Int32.MaxValue);
      }

      public TimeSpan RunReverse ()
      {
         return Step(Int32.MinValue);
      }
   }
}
