using System.IO;
using System.Linq;
using System.Collections.Generic;
using System;
using System.Collections.Concurrent;

namespace Eclat
{
    public class RawItem
    {
        readonly char[] delimiters = new char[] { ',' };

        public int user { get; set; }

        public string[] artists { get; set; }

        public RawItem(string str)
        {
            var elements = str.Split(delimiters, 2, StringSplitOptions.RemoveEmptyEntries);
           
            user = int.Parse(elements[0]);
            artists = elements[1].Split(delimiters, StringSplitOptions.RemoveEmptyEntries);
        }
    }

    public class CSVReader
    {
        public CSVReader() { }

        // Чтнение файла данных
        public List<string> read(string path)
        {
            return File.ReadAllLines(path).AsParallel().AsParallel()
                .WithMergeOptions(ParallelMergeOptions.FullyBuffered).ToList();
        }

        // Обработка строк из файла данных
        public (Dictionary<String, HashSet<int>>, int) parse(List<string> records)
        {
            var rawItems = Partitioner.Create(records, true)
                .AsParallel()
                .WithMergeOptions(ParallelMergeOptions.FullyBuffered)
                .Select(record => new RawItem(record))
                .SelectMany(item => item.artists.Select(artist => (item.user, artist)));

            var dataset = rawItems.Aggregate(
                () => new Dictionary<String, HashSet<int>>(),
                (dic, item) =>
                {
                    if (dic.Keys.Contains(item.artist))
                        dic[item.artist].Add(item.user);
                    else
                        dic.Add(item.artist, new HashSet<int> { item.user });

                    return dic;
                },
                (dic1, dic2) =>
                {
                    foreach (var kv in dic2)
                    {
                        if (dic1.Keys.Contains(kv.Key))
                            dic1[kv.Key].UnionWith(kv.Value);
                        else
                            dic1.Add(kv.Key, kv.Value);
                    }

                    return dic1;
                },
                final => final);

            return (dataset, records.Count);
        }
            
    }
}
