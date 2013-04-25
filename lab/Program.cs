using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;

namespace Lab
{
   class Program
   {
      private static List<Test> tests = new List<Test>();
      private static List<Clock> clocks = new List<Clock>();
      private static Clock realClock = new Clock();

      static Int32 Main (String[] options)
      {
         Console.WriteLine("Test Laboratory");
         if (!ParseOptions(options))
         {
            ReportUsage();
            return 1;
         }
         ExecuteTests();
         return 0;
      }

      static Boolean ParseOptions (String[] options)
      {
         // parse options
         try
         {
            new Options.OptionSet()
            {
               { "t|threads=", (Int32 v) => Test.Threads = v },
               { "i|iterations=", (Int32 v) => Test.Iterations = v },
               { "p|Test.Param=", v => Test.Param = v }
            }.Parse(options);
         }
         catch { return false; }
         // validate options
         if (Test.Threads <= 0)
            return false;
         if (Test.Iterations <= 0)
            return false;
         return true;
      }

      static void ReportUsage ()
      {
         Console.WriteLine("   Usage: Lab {options}");
         Console.WriteLine("      -t|-threads {count}        number of threads to run");
         Console.WriteLine("      -i|-iterations {count}     number of iterations per thread to run");
         Console.WriteLine("      -p|-Test.Param {value}          custom test paramter");
      }

      static void ExecuteTests ()
      {
         Console.WriteLine(
            "Running {0} threads, {1} iterations.",
            Test.Threads,
            Test.Iterations
         );
         Console.WriteLine();
         var threads = new List<Thread>();
         for (var i = 0; i < Test.Threads; i++)
         {
            tests.Add(new Test() { ThreadID = i });
            threads.Add(new Thread(ExecuteThread));
            clocks.Add(new Clock());
         }
         realClock.Start();
         for (var i = 0; i < Test.Threads; i++)
            threads[i].Start(i);
         for (var i = 0; i < Test.Threads; i++)
            threads[i].Join();
         realClock.Stop();
         Console.WriteLine();
         Console.WriteLine("Performance:");
         Console.WriteLine("   Runs:  {0,12}", clocks.Runs());
         Console.WriteLine("   Real:  {0,12:0.000} s", realClock.MeanTime);
         Console.WriteLine("   Min:   {0,12:0.000} ms", clocks.MinTime() * 1000);
         Console.WriteLine("   Max:   {0,12:0.000} ms", clocks.MaxTime() * 1000);
         Console.WriteLine("   Mean:  {0,12:0.000} ms", clocks.MeanTime() * 1000);
         Console.WriteLine("   StdDev:{0,12:0.000} ms", clocks.StdDevTime() * 1000);
         if (Debugger.IsAttached)
         {
            Console.WriteLine();
            Console.Write("Press enter to exit.");
            Console.ReadLine();
         }
      }

      static void ExecuteThread (Object param)
      {
         var t = (Int32)param;
         var test = tests[t];
         var clock = clocks[t];
         for (test.Iteration = 0; test.Iteration < Test.Iterations; test.Iteration++)
         {
            clock.Start();
            test.Run();
            clock.Stop();
         }
      }
   }
}
