using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Threading;

namespace Lab_2
{
    class Program
    {
        static int count = 10000;
        static int sqrtn = (int)Math.Truncate(Math.Sqrt(count));

        static int finishCount = 0; // счетчик завершенных задач

        static double totalTime = 0.0; // время работы

        static int[] baseArray = new int[count];
        static List<int> baseSimpleList = new List<int>(); // найденные первые простые числа

        static bool[] checkArray = new bool[baseArray.Length];

        static object sync = new object();

        static void Main(string[] args)
        {
            Thread.CurrentThread.CurrentCulture = new System.Globalization.CultureInfo("de-DE"); // чтобы разделителем дробных чисел была запятая
            Stopwatch watch = new Stopwatch();

            for (int k = 0; k < 5; k++)
            {
                for (int i = 0; i < count; i++)
                {
                    baseArray[i] = i + 1;
                    checkArray[i] = false;
                }
                baseSimpleList.Clear();
                watch.Start();

                // ЭТАП 1

                for (int i = 1; i < sqrtn; i++)
                {
                    if (!checkArray[i])
                    {
                        findSimpleFor(i + 1, i + 1, sqrtn);
                        baseSimpleList.Add(i + 1);
                    }
                }

                // ЭТАП 2

                finishCount = 0;

                for (int i = 0; i < baseSimpleList.Count; i++)
                {
                    ThreadPool.QueueUserWorkItem(findSimpleForObject, new object[] { baseSimpleList[i] });
                }

                while (true)
                {
                    lock (sync)
                    {
                        if (finishCount >= baseSimpleList.Count) break;
                    }

                    // ожидаем
                }

                watch.Stop();
                totalTime += k == 0 ? 0 : watch.Elapsed.TotalMilliseconds;
                watch.Reset();
            }
            Console.WriteLine(totalTime);

            for (int i = 1; i < count; i++)
            {
                if (!checkArray[i]) Console.Write("{0} ", baseArray[i]);
            }
        }

        static void findSimpleForObject(object p)
        {
            int param = (int)((object[])p)[0];

            findSimpleFor(param, sqrtn, count);
            Interlocked.Increment(ref finishCount);
        }

        static void findSimpleFor(int num, int from, int to)
        {
            for (int i = from; i < to; i++)
            {
                if (!checkArray[i] && baseArray[i] % num == 0) checkArray[i] = true;
            }
        }
    }
}
