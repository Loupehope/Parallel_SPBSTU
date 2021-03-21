#pragma once

#include <vector>

class SerialSort
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
	SerialSort(int valuesCount, DataType dataType);

	int* start();

private:
	void setup();
	void serialEnumSort(int valuesCount, int from, int to);

	int* generateArray(DataType type, int count);
};

