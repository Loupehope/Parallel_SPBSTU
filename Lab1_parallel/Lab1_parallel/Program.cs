using System;
using System.Diagnostics;
using System.Threading;

namespace Lab1_parallel
{
    class Program
    {
        static int count = 1000000;
        static int threadsCount = 10;

        static int[] fromArray = new int[count];
        static int[] toArray = new int[fromArray.Length];

        static void Main(string[] args)
        {
            Thread.CurrentThread.CurrentCulture = new System.Globalization.CultureInfo("de-DE"); // чтобы разделителем дробных чисел была запятая

            Thread[] threads = new Thread[threadsCount];

            for (int i = 0; i < fromArray.Length; i++) fromArray[i] = 0;

            for (int i = 0; i < threads.Length; i++)
            {
                threads[i] = new Thread(performSum);
            }

            Stopwatch watch = new Stopwatch();
            watch.Start();

            for (int i = 0; i < threadsCount; i++)
                threads[i].Start(new int[] { i * (count / threadsCount), (i + 1) * (count / threadsCount) });

            /* Разделение по диапазону при при круговом разделении элементов вектора 
            for (int i = 0; i < threadsCount; i++)
               threads[i].Start(new int[] { i });
            */

            for (int i = 0; i < threadsCount; i++) threads[i].Join();
            watch.Stop();

            Console.WriteLine(watch.Elapsed.TotalMilliseconds);
        }

        // Многопоточная обработка элементов вектора, используя разделение вектора на равное число элементов
        static void performSum(object input)
        {
            int[] config = (int[])input;
            int minIndex = config[0];
            int maxIndex = config[1];

            for(int i = minIndex; i < maxIndex; i++)
                toArray[i] = fromArray[i] + 10;
        }

        // Усложнение обработки каждого элемента вектора
        static void performLongSumForObject(object input)
        {
            int[] config = (int[])input;
            int minIndex = config[0];
            int maxIndex = config[1];

            for (int i = minIndex; i < maxIndex; i++)
            {
                for (int j = 0; j < 1000; j++)
                {
                    toArray[i] = fromArray[i] + 10;
                }
            }
        }

        // Разделение по диапазону при неравномерной вычислительной сложности обработки элементов вектора
        static void performRangeSumForObject(object input)
        {
            int[] config = (int[])input;
            int minIndex = config[0];
            int maxIndex = config[1];

            for (int i = minIndex; i < maxIndex; i++)
            {
                for (int j = 0; j < i; j++)
                {
                    toArray[i] = fromArray[i] + 1;
                }
            }
        }

        // Разделение по диапазону при при круговом разделении элементов вектора
        static void performCircleSumForObject(object input)
        {
            int[] config = (int[])input;
            int minIndex = config[0];

            for (int i = minIndex; i < fromArray.Length; i += threadsCount)
            {
                for (int j = 0; j < i; j++)
                {
                    toArray[i] = fromArray[i] + 1;
                }
            }
        }
    }
}
