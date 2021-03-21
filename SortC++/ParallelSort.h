#pragma once

#include <vector>

class ParallelSort
{
public:
	enum class DataType
	{
		Random,
		Asc,
		Desc
	};

private:
	int* sortedValues;
	int* values;

	int valuesCount = 0;

	DataType dataType = static_cast<DataType>(0);

public:
	ParallelSort(int valuesCount, DataType dataType);

	int* start();

private:
	void setup();
	void parallelEnumSort(int valuesCount, int from, int to);
	void serialEnumSort(int valuesCount, int i);

	int* generateArray(DataType type, int count);
};

