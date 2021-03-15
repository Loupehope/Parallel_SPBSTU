using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using System.Linq;
using System.Threading.Tasks;

namespace Eclat
{
    using ItemsSet = KeyValuePair<HashSet<String>, HashSet<int>>;
    using Rules = ConcurrentDictionary<HashSet<String>, double>;

    // MARK: - Eclat

    public class EclatParallel
    {
        /*  Item     |  Transaction ids set
         *  ------------------------------------
         * "Bread"   |  1, 2, 3, 4, 6
         */

        private Dictionary<String, HashSet<int>> dataset;

        /*  Правила: "Bread", "Jam": 1.0 */

        private Rules rules;

        /*  Предметы: "Bread", "Jam" ... */

        private List<String> items;

        public EclatParallel() { initialize(); }

        // Параллельная реализация Eclat
        public Rules StartProcess(Dictionary<String, HashSet<int>> inputDataset, int dataSize, double minSupport,
            IProgress<int> progress, CancellationToken token)
        {
            initialize();

            double scaledMinimalSupport = minSupport * dataSize;

            dataset = inputDataset;
            items = new List<string>(dataset.Keys);

            int counter = 0;

            Enumerable.Range(0, items.Count).AsParallel().WithCancellation(token).ForAll(i =>
            {
                string item = items[i];

                HashSet<string> newKey = new HashSet<string>() { item };
                ItemsSet newItem = new ItemsSet(newKey, dataset[item]);

                eclat(newItem, scaledMinimalSupport, i, dataSize);

                lock ("report")
                {
                    counter++;
                    progress.Report(counter);
                }
            });

            return rules;
        }

        // Реализация алгоритма Eclat
        private void eclat(ItemsSet itemsSet, double minSupport, int index, double dataSize)
        {
            if (itemsSet.Value.Count < minSupport) return;

            for (int i = index + 1; i < items.Count; i++)
            {
                var availableItem = items[i];

                if (dataset[availableItem].Count < minSupport) continue;

                var newItemsSet = new ItemsSet(new HashSet<string>(itemsSet.Key), new HashSet<int>(itemsSet.Value));

                newItemsSet.Key.Add(availableItem);
                newItemsSet.Value.IntersectWith(dataset[availableItem]);

                double newItemsSupport = newItemsSet.Value.Count;

                if (newItemsSupport >= minSupport)
                {
                    if (!rules.TryAdd(newItemsSet.Key, newItemsSupport / dataSize)) continue;

                    eclat(newItemsSet, minSupport, i, dataSize);
                }
            }
        }

        private void initialize()
        {
            rules = new Rules();
            items = new List<string>();
        }
    }
}
