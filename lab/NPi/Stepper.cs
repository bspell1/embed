using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace NPi
{
   public class Stepper
   {
      public const Int32 MinRpm = 6;
      public const Int32 MaxRpm = 60;
      private IStepperDriver driver;

      public Stepper (IStepperDriver driver)
      {
         this.driver = driver;
      }

      public Int32 StepsPerCycle
      {
         get { return this.driver.StepsPerCycle; }
         set { this.driver.StepsPerCycle = value; }
      }

      public Int32 Rpm
      {
         get; set;
      }

      public void Stop ()
      {
         this.driver.Stop();
      }

      public void Step (Int32 steps)
      {
         if (this.StepsPerCycle == 0)
            throw new InvalidOperationException("Invalid StepsPerCycle");
         if (this.Rpm < MinRpm || this.Rpm > MaxRpm)
            throw new InvalidOperationException(
               String.Format("Invalid RPM: valid range is [{0}, {1}]", MinRpm, MaxRpm)
            );
         this.driver.Step(steps, this.Rpm);
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
