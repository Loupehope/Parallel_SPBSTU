using System;
using System.Diagnostics;
using System.Threading;

namespace Lab1_parallel
{
    class Program
    {
        static int[] fromArray;
        static int[] toArray;

        static void Main(string[] args)
        {
            int count = 10; // from array capacity
            int threadsCount = 1;

            fromArray = new int[count];
            toArray = new int[fromArray.Length];

            Thread[] threads = new Thread[threadsCount];

            // Fill from array with zeros
            for (int i = 0; i < fromArray.Length; i++) fromArray[i] = 0;

            for (int i = 0; i < threads.Length; i++) threads[i] = new Thread(performSumForObject);

            Stopwatch watch = new Stopwatch();
            watch.Start();

            for (int i = 0; i < threadsCount; i++)
                threads[i].Start(new int[] { i * (count / threadsCount), (i + 1) * (count / threadsCount) });

            for (int i = 0; i < threadsCount; i++) threads[i].Join();
            watch.Stop();

            Console.WriteLine("Total time: {0}", watch.Elapsed.TotalMilliseconds);
        }

        static void performSumForObject(object input)
        {
            int[] config = (int[])input;
            int minIndex = config[0];
            int maxIndex = config[1];

            for(int i = minIndex; i < maxIndex; i++)
            {
                toArray[i] = fromArray[i] + 10;
            }
        }
    }
}
