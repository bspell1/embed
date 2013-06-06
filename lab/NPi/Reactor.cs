using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Threading;
using Mono.Unix.Native;

namespace NPi
{
   public class Reactor : IDisposable
   {
      private Thread thread;
      private Boolean shutdown;
      private Int32 epoll = -1;
      private List<Tuple<Func<Boolean>, Action>> pollers;
      private Dictionary<Int32, Action> selectors;
      private EpollEvent[] events;

      public Reactor ()
      {
         this.Interval = TimeSpan.FromMilliseconds(1);
      }

      public void Dispose ()
      {
         Join();
         if (this.epoll != -1)
            Syscall.close(this.epoll);
         this.epoll = -1;
      }

      public TimeSpan Interval
      {
         get; set; 
      }

      public void Poll (Func<Boolean> cond, Action action)
      {
         lock (this)
         {
            if (this.pollers == null)
               this.pollers = new List<Tuple<Func<Boolean>, Action>>();
            this.pollers.Add(Tuple.Create(cond, action));
         }
      }

      public void Select (Int32 fd, Action action)
      {
         lock (this)
         {
            // register the file descriptor with epoll
            if (this.epoll == -1)
            {
               this.epoll = Syscall.epoll_create((EpollFlags)0);
               if (this.epoll == -1)
                  throw new Win32Exception();
            }
            if (Syscall.epoll_ctl(
                  this.epoll,
                  EpollOp.EPOLL_CTL_ADD,
                  fd,
                  EpollEvents.EPOLLIN | EpollEvents.EPOLLET
               ) < 0)
               throw new Win32Exception();
            // add the action to the descriptor map
            if (this.selectors == null)
               this.selectors = new Dictionary<Int32, Action>();
            this.selectors.Add(fd, action);
            // allocate the event wait structure
            var evt = new EpollEvent() { fd = fd };
            Array.Resize(ref this.events, this.selectors.Count);
            this.events[this.events.Length - 1] = evt;
            //===============================================================
            // KLUDGE: bspell - 6/5/2013
            // . upon adding a file descriptor to an epoll set,
            //   the kernel raises the event, even though no edge
            //   transition has occurred
            // . wait for this initial event here to avoid propagating
            //   these bogus events
            //===============================================================
            Syscall.epoll_wait(this.epoll, new[] { evt }, 1, 0);
         }
      }

      public void Run ()
      {
         while (!this.shutdown)
         {
            lock (this)
            {
               // poll any registered pollers
               if (this.pollers != null)
                  foreach (var poller in this.pollers)
                     if (poller.Item1())
                        poller.Item2();
               // wait for any registered selectors
               if (this.epoll != -1)
               {
                  var select = Syscall.epoll_wait(
                     this.epoll,
                     this.events,
                     this.events.Length,
                     (Int32)this.Interval.TotalMilliseconds
                  );
                  if (select < 0)
                     throw new Win32Exception();
                  for (var i = 0; i < select; i++)
                     this.selectors[this.events[i].fd]();
               }
               else
                  Thread.Sleep(this.Interval);
            }
         }
      }
      public void Start ()
      {
         Join();
         this.shutdown = false;
         this.thread = new Thread(Run);
         this.thread.Start();
      }
      public void Join ()
      {
         if (this.thread != null)
         {
            this.shutdown = true;
            this.thread.Join();
            this.thread = null;
         }
      }
   }
}
