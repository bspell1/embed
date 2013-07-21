using System;
using System.Collections.Generic;
using System.Linq;

namespace NPi
{
   public class StepTrike
   {
      private StepMotor left;
      private StepMotor right;
      private Double wheelRadius;
      private Double axleWidth;

      public StepTrike (
         StepMotor left, 
         StepMotor right,
         Double wheelRadius,
         Double axleWidth)
      {
         this.left = left;
         this.right = right;
         this.wheelRadius = wheelRadius;
         this.axleWidth = axleWidth;
      }

      public StepMotor Left { get { return this.left; } }
      public StepMotor Right { get { return this.right; } }
      public Int32 MaxRpm { get { return this.left.MaxRpm; } }

      public TimeSpan Stop ()
      {
         this.left.Stop();
         return this.right.Stop();
      }

      public TimeSpan Run (Int32 leftRpm, Int32 rightRpm)
      {
         this.left.Rpm = leftRpm;
         this.right.Rpm = rightRpm;
         this.left.Run();
         this.right.Run();
         return TimeSpan.MaxValue;
      }
      public TimeSpan Run (Double distance, Int32 rpm)
      {
         var steps = GetStepCount(distance);
         this.left.Rpm = this.right.Rpm = rpm;
         this.left.Step(steps);
         return this.right.Step(steps);
      }

      public TimeSpan Rotate (Int32 rpm, Double radians)
      {
         return Revolve(0, rpm, radians);
      }
      public TimeSpan RotateCW (Int32 rpm)
      {
         return Run(rpm, -rpm);
      }
      public TimeSpan RotateCCW (Int32 rpm)
      {
         return Run(-rpm, rpm);
      }
      public TimeSpan Revolve (Double radius, Int32 rpm, Double radians)
      {
         var oSteps = GetStepCount(Math.Abs(radians * (radius + this.axleWidth / 2)));
         var iSteps = GetStepCount(Math.Abs(radians * (radius - this.axleWidth / 2)));
         var oRpm = rpm;
         var iRpm = (Int32)Round(
            rpm * ((radius - this.axleWidth / 2) / (radius + this.axleWidth / 2))
         );
         if (radians < 0)
         {
            // CW
            this.left.Rpm = oRpm;
            this.right.Rpm  = iRpm;
            this.right.Step(iSteps);
            return this.left.Step(oSteps);
         }
         else
         {
            // CCW
            this.left.Rpm = iRpm;
            this.right.Rpm = oRpm;
            this.left.Step(iSteps);
            return this.right.Step(oSteps);
         }
      }
      public TimeSpan RevolveCW (Double radius, Int32 rpm)
      {
         return Run(
            rpm, 
            (Int32)(
               rpm * 
               (radius - this.axleWidth / 2) / (radius + this.axleWidth / 2)
            )
         );
      }
      public TimeSpan RevolveCCW (Double radius, Int32 rpm)
      {
         return Run(
            (Int32)(
               rpm *
               (radius - this.axleWidth / 2) / (radius + this.axleWidth / 2)
            ),
            rpm
         );
      }
      private Int32 GetStepCount (Double distance)
      {
         return (Int32)Round(
            (distance / (2 * Math.PI * this.wheelRadius)) *
            this.right.StepsPerCycle
         );
      }
      private Int32 Round (Double value)
      {
         return (Int32)(value + 0.5);
      }
   }
}
