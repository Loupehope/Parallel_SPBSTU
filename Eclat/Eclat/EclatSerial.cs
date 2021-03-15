﻿using System;
using System.Collections.Generic;
using System.Threading;

namespace Eclat
{
    using ItemsSet = KeyValuePair<HashSet<String>, HashSet<int>>;
    using Rules = Dictionary<HashSet<String>, double>;

    // MARK: - Eclat

    public class EclatSerial
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

        public EclatSerial() { initialize(); }

        // Последовательная реализация Eclat
        public Rules StartProcess(Dictionary<String, HashSet<int>> inputDataset, int dataSize, double minSupport,
            IProgress<int> progress, CancellationToken token)
        {
            initialize();

            double scaledMinimalSupport = minSupport * dataSize;

            this.dataset = inputDataset;
            this.items = new List<string>(dataset.Keys);

            for (int i = 0; i < items.Count; i++)
            {
                string item = items[i];

                HashSet<string> newKey = new HashSet<string>() { item };
                ItemsSet newItem = new ItemsSet(newKey, dataset[item]);

                eclat(newItem, scaledMinimalSupport, i, dataSize, token);

                if (progress != null)
                {
                    progress.Report(i);
                }
            }

            return rules;
        }

        // Реализация алгоритма Eclat
        private void eclat(ItemsSet itemsSet, double minSupport, int index, double dataSize, CancellationToken token)
        {
            if (itemsSet.Value.Count < minSupport) return;

            for (int i = index + 1; i < items.Count; i++)
            {
                token.ThrowIfCancellationRequested();

                var availableItem = items[i];

                if (dataset[availableItem].Count < minSupport) continue;

                var newItemsSet = new ItemsSet(new HashSet<string>(itemsSet.Key), new HashSet<int>(itemsSet.Value));

                newItemsSet.Key.Add(availableItem);
                newItemsSet.Value.IntersectWith(dataset[availableItem]);

                double newItemsSupport = newItemsSet.Value.Count;

                if (newItemsSupport >= minSupport)
                {
                    rules.Add(newItemsSet.Key, newItemsSupport / dataSize);
                    eclat(newItemsSet, minSupport, i, dataSize, token);
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
