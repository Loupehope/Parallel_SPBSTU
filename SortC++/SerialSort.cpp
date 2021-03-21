#include "SerialSort.h"

SerialSort::SerialSort(int valuesCount, DataType dataType)
{
	this->valuesCount = valuesCount;
	this->dataType = dataType;

	setup();
}

int* SerialSort::start()
{
	serialEnumSort(valuesCount, 0, valuesCount);

	return sortedValues;
}

void SerialSort::serialEnumSort(int valuesCount, int from, int to)
{
	int x;
	// по всем элементам
	for (int i = from; i < to; i++)
	{
		x = 0;
		// вычисляем ранг элемента
		for (int j = 0; j < valuesCount; j++)
		{
			if (values[i] > values[j] || (values[i] == values[j] && j > i)) x++;
		}

		int value = values[i];
		sortedValues[x] = value; // записываем в результирующую
	}
}

void SerialSort::setup()
{
	values = generateArray(dataType, valuesCount);
	sortedValues = new int[valuesCount];
}

int* SerialSort::generateArray(DataType type, int count)
{
	int* array = new int[count];

	switch (type) {
	case DataType::Random:
	{
		for (int i = 0; i < count; i++) array[i] = rand() % 101;
		break;
	}

	case DataType::Asc:
		for (int i = 0; i < count; i++) array[i] = i;
		break;

	case DataType::Desc:
		for (int i = 0; i < count; i++) array[i] = count - i;
		break;
	}
	return array;
}