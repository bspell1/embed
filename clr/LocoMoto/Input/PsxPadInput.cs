using System;
using System.Collections.Generic;
using System.Linq;

using NPi;
using NPi.Psx;

namespace LocoMoto
{
   public class PsxPadInput : IInput, IDisposable
   {
      private Nrf24 receiver;
      private IPsxPadReceiver psx;
      private Controller control;
      private PsxPadState state;

      public event Action Changed;

      public PsxPadInput (Config config, Reactor reactor, Controller controller)
      {
         try
         {
            this.receiver = new Nrf24(config.SpiPath, config.CEPin, config.IrqPin, reactor);
            this.psx = new Nrf24Receiver(this.receiver, config.Address, config.Pipe);
            this.control = controller;
            this.receiver.Config = new Nrf24.ConfigRegister(this.receiver.Config)
            {
               Mode = Nrf24.Mode.Receive,
               Crc = Nrf24.Crc.TwoByte
            };
            this.receiver.Listen();
            this.psx.Received += this.OnPsxReceived;
         }
         catch
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.psx != null && this.psx is IDisposable)
            ((IDisposable)this.psx).Dispose();
         this.psx = null;
         if (this.receiver != null)
            this.receiver.Dispose();
         this.receiver = null;
      }

      private void OnPsxReceived (PsxPadState state)
      {
         var changed = false;
         if (this.state.R2 != state.R2)
         {
            OnR2(state.R2);
            changed = true;
         }
         state.LX = (state.LX / 10.0f) * 10.0f;
         state.LY = (state.LY / 10.0f) * 10.0f;
         state.RX = (state.RX / 10.0f) * 10.0f;
         state.RY = (state.RY / 10.0f) * 10.0f;
         if (Math.Abs(this.state.LX - state.LX) > PsxPadState.JoystickEpsilon ||
             Math.Abs(this.state.LY - state.LY) > PsxPadState.JoystickEpsilon ||
             Math.Abs(this.state.RX - state.RX) > PsxPadState.JoystickEpsilon ||
             Math.Abs(this.state.RY - state.RY) > PsxPadState.JoystickEpsilon)
         {
            OnJoystick(state);
            changed = true;
         }
         if (changed && this.Changed != null)
            this.Changed();
         this.state = state;
      }

      private void OnJoystick (PsxPadState state)
      {
         this.control.Trike.Run(
            -(Int32)(state.LY * control.Trike.MaxRpm),
            -(Int32)(state.RY * control.Trike.MaxRpm)
         );
      }

      private void OnR2 (Boolean value)
      {
         this.control.SetOutput(0, value);
      }

      public struct Config
      {
         public String SpiPath { get; set; }
         public Int32 CEPin { get; set; }
         public Int32 IrqPin { get; set; }
         public Int32 Pipe { get; set; }
         public String Address { get; set; }
      }
   }
}
