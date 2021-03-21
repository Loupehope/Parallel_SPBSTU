// SortC.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <ctime>
#include <omp.h>
#include <ratio>
#include <chrono>

#include "ParallelSort.h"
#include "SerialSort.h"

int main()
{
	int valuesCount = 100000;
	auto type = SerialSort::DataType::Random;
	auto ptype = ParallelSort::DataType::Random;
	int t = 2;

	SerialSort* sSort = new SerialSort(valuesCount, type);
	ParallelSort* pSort = new ParallelSort(valuesCount, ptype);

	double time = 0;

	for (int i = 0; i < 3; i++) {
		auto start = std::chrono::high_resolution_clock::now();
		int* res1 = sSort->start();
		auto end = std::chrono::high_resolution_clock::now();
		time += i == 0 ? 0 : std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	}

	std::cout << "Serial: " << valuesCount << " " << t << ": " << time / 2 << " seconds.";
	std::cout << std::endl; 

	time = 0;

	for (int i = 0; i < 3; i++) {
		auto start = std::chrono::high_resolution_clock::now();
		int* res2 = pSort->start();
		auto end = std::chrono::high_resolution_clock::now();
		time += i == 0 ? 0 : std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	}

	std::cout << "Parallel: " << valuesCount << " " << t << ": " << time / 2 << " seconds.";
	std::cout << std::endl;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
