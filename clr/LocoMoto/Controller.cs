using System;
using System.Collections.Generic;
using System.Linq;

using NPi;

namespace LocoMoto
{
   public class Controller : IDisposable
   {
      private Protocol proto;

      public Controller (Config config)
      {
         this.proto = new Protocol(config.UartPath, config.LocoAddress);
         this.Trike = new StepTrike(
            new StepMotor(new StepperDriver(this.proto, 0), config.TrikeMinRpm, config.TrikeMaxRpm)
            {
               StepsPerCycle = config.TrikeStepsPerCycle,
               Reverse = true
            },
            new StepMotor(new StepperDriver(this.proto, 1), config.TrikeMinRpm, config.TrikeMaxRpm)
            {
               StepsPerCycle = config.TrikeStepsPerCycle
            },
            config.TrikeWheelRadius,
            config.TrikeAxleWidth
         );
      }

      public void Dispose ()
      {
         if (this.proto != null)
            this.proto.Dispose();
      }

      public StepTrike Trike { get; private set; }

      public void SetOutput (Int32 channel, Boolean value)
      {
         this.proto.SetOutput(channel, value);
      }

      public struct Config
      {
         public String UartPath { get; set; }
         public Int32 LocoAddress { get; set; }
         public Int32 TrikeStepsPerCycle { get; set; }
         public Int32 TrikeMinRpm { get; set; }
         public Int32 TrikeMaxRpm { get; set; }
         public Double TrikeWheelRadius { get; set; }
         public Double TrikeAxleWidth { get; set; }
      }

      private class StepperDriver : IStepperDriver
      {
         private Protocol proto;
         private Int32 motor;

         public StepperDriver (Protocol proto, Int32 motor)
         {
            this.proto = proto;
            this.motor = motor;
         }

         #region IStepperDriver Implementation
         public Int32 StepsPerCycle
         {
            get; set;
         }
         public TimeSpan Stop ()
         {
            this.proto.MotorStop(this.motor);
            return TimeSpan.FromMilliseconds(1);
         }
         public TimeSpan Step (Int32 steps, Int32 rpm)
         {
            if (steps == Int32.MaxValue)
               steps = Int16.MaxValue;
            else if (steps >= Int16.MaxValue)
               throw new ArgumentOutOfRangeException("steps");
            if (steps == Int32.MinValue)
               steps = Int16.MinValue;
            else if (steps <= Int16.MinValue)
               throw new ArgumentOutOfRangeException("steps");
            if (rpm < 0)
            {
               steps *= -1;
               rpm *= -1;
            }
            var delay = 600000 / (rpm * this.StepsPerCycle * 4);
            this.proto.MotorStep(this.motor, steps, delay);
            return steps != Int16.MinValue && steps != Int16.MaxValue ?
               TimeSpan.FromMilliseconds(steps * (delay / 10)) :
               TimeSpan.MaxValue;
         }
         #endregion
      }
   }
}
