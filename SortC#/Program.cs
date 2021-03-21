using System;
using System.Diagnostics;

namespace Csharp
{
    public class Program
    {
        static void Main(string[] args)
        {
            int valuesCount = 100000;
            var type = SerialSort.DataType.Random;
            var ptype = ParallelSort.DataType.Random;

            Stopwatch watch = new Stopwatch();
            SerialSort sSort = new SerialSort(valuesCount, type);
            ParallelSort pSort = new ParallelSort(valuesCount, ptype);

            double time = 0;

            for (int i = 0; i < 3; i++)
            {
                watch.Restart();
               // sSort.start();
                watch.Stop();
                time += i == 0 ? 0 : watch.Elapsed.TotalSeconds;
            }

            Console.WriteLine();
            Console.WriteLine("Serial " + valuesCount + " " + type + ": " + time / 2);

            time = 0;

            for (int i = 0; i < 3; i++)
            {
                watch.Restart();
                pSort.start();
                watch.Stop();
                time += i == 0 ? 0 : watch.Elapsed.TotalSeconds;
            }

            Console.WriteLine();
            Console.WriteLine("Parallel " + valuesCount + " " + type + ": " + time / 2);

        }
    }
}
