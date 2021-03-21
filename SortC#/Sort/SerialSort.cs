using System;

namespace Csharp
{
    public class SerialSort
    {
        public enum DataType
        {
            Random,
            Asc,
            Desc
        }

        private int[] sortedValues;
        private int[] values;

        private int valuesCount;

        private DataType dataType;

        public SerialSort(int valuesCount, DataType dataType)
        {
            this.valuesCount = valuesCount;
            this.dataType = dataType;

            setup();
        }

        public int[] start()
        {
            serialEnumSort(valuesCount, 0, valuesCount);

            return sortedValues;
        }

        private void serialEnumSort(int valuesCount, int from, int to)
        {
            int x;
            // по всем элементам
            for (int i = from; i < to; i++)
            {
                x = 0;
                // вычисляем ранг элемента
                for (int j = 0; j < valuesCount; j++)
                    if (values[i] > values[j] || (values[i] == values[j] && j > i)) x++;

                sortedValues[x] = values[i]; // записываем в результирующую
            }
        }

        private void setup()
        {
            values = generateArray(dataType, valuesCount);
            sortedValues = new int[valuesCount];
        }

        private int[] generateArray(DataType type, int count)
        {
            int[] array = new int[count];

            switch (type)
            {
                case DataType.Random:
                    Random rand = new Random();
                    for (int i = 0; i < count; i++)
                        array[i] = rand.Next(0, 100);
                    break;
                case DataType.Asc:
                    for (int i = 0; i < count; i++)
                        array[i] = i;
                    break;
                case DataType.Desc:
                    for (int i = 0; i < count; i++)
                        array[i] = count - i;
                    break;
            }
            return array;
        }
    }
}
