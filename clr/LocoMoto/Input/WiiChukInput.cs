using System;
using System.Collections.Generic;
using System.Linq;

using NPi;
using NPi.WiiChuk;

namespace LocoMoto
{
   public class WiiChukInput : IInput, IDisposable
   {
      private Nrf24 receiver;
      private WiiChukPair chuks;
      private Controller control;
      private WiiChukState left = WiiChukState.Zero;
      private WiiChukState right = WiiChukState.Zero;

      public event Action Changed;

      public WiiChukInput (Config config, Reactor reactor, Controller controller)
      {
         try
         {
            this.receiver = new Nrf24(config.SpiPath, config.CEPin, config.IrqPin, reactor);
            this.chuks = new WiiChukPair(
               new Nrf24Receiver(this.receiver, config.Address, config.Pipe)
            );
            this.control = controller;
            this.receiver.Config = new Nrf24.ConfigRegister(this.receiver.Config)
            {
               Mode = Nrf24.Mode.Receive,
               Crc = Nrf24.Crc.TwoByte
            };
            this.receiver.Listen();
            this.chuks.Updated += this.OnChukUpdated;
         }
         catch
         {
            Dispose();
            throw;
         }
      }

      public void Dispose ()
      {
         if (this.chuks != null)
            this.chuks.Dispose();
         this.chuks = null;
         if (this.receiver != null)
            this.receiver.Dispose();
         this.receiver = null;
      }

      private void OnChukUpdated (WiiChukState left, WiiChukState right)
      {
         var changed = false;
         if (this.left.CButton != left.CButton)
         {
            OnLeftCButton(left.CButton);
            changed = true;
         }
         if (this.left.ZButton != left.ZButton)
         {
            OnLeftZButton(left.ZButton);
            changed = true;
         }
         if (this.right.CButton != right.CButton)
         {
            OnRightCButton(right.CButton);
            changed = true;
         }
         if (this.right.ZButton != right.ZButton)
         {
            OnRightZButton(right.ZButton);
            changed = true;
         }
         if (Math.Abs(this.left.JoystickX - left.JoystickX) > WiiChukState.JoystickEpsilon ||
             Math.Abs(this.left.JoystickY - left.JoystickY) > WiiChukState.JoystickEpsilon ||
             Math.Abs(this.right.JoystickX - right.JoystickX) > WiiChukState.JoystickEpsilon ||
             Math.Abs(this.right.JoystickY - right.JoystickY) > WiiChukState.JoystickEpsilon
            )
         {
            OnJoystick(left, right);
            changed = true;
         }
         if (changed && this.Changed != null)
            this.Changed();
         this.left = left;
         this.right = right;
      }

      private void OnJoystick (WiiChukState left, WiiChukState right)
      {
         this.control.Trike.Run(
            (Int32)(left.JoystickY * control.Trike.MaxRpm),
            (Int32)(right.JoystickY * control.Trike.MaxRpm)
         );
      }

      private void OnLeftCButton (Boolean value)
      {
      }

      private void OnRightCButton (Boolean value)
      {
      }

      private void OnLeftZButton (Boolean value)
      {
      }

      private void OnRightZButton (Boolean value)
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
