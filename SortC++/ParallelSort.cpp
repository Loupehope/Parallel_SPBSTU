#include "ParallelSort.h"
#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

ParallelSort::ParallelSort(int valuesCount, DataType dataType)
{
	this->valuesCount = valuesCount;
	this->dataType = dataType;

	setup();
}

int* ParallelSort::start()
{
	parallelEnumSort(valuesCount, 0, valuesCount);

	return sortedValues;
}

void ParallelSort::parallelEnumSort(int valuesCount, int from, int to)
{
	#pragma omp parallel for num_threads(8)
	// по всем элементам
	for (int i = from; i < to; i++)
	{
		ParallelSort::serialEnumSort(valuesCount, i);
	}
}

void ParallelSort::serialEnumSort(int valuesCount, int i)
{
	int x = 0;
	// вычисляем ранг элемента
	for (int j = 0; j < valuesCount; j++)
	{
		if (values[i] > values[j] || (values[i] == values[j] && j > i)) x++;
	}

	int value = values[i];
	sortedValues[x] = value; // записываем в результирующую
}

void ParallelSort::setup()
{
	values = generateArray(dataType, valuesCount);

	sortedValues = new int[valuesCount];
}

int* ParallelSort::generateArray(DataType type, int count)
{
	int* array = new int[count];

	switch (type)
	{
	case DataType::Random:
	{
		for (int i = 0; i < count; i++)
		{
			array[i] = rand() % 101;
		}
		break;
	}

	case DataType::Asc:
		for (int i = 0; i < count; i++)
		{
			array[i] = i;
		}
		break;

	case DataType::Desc:
		for (int i = 0; i < count; i++)
		{
			array[i] = count - i;
		}
		break;
	}
	return array;
}