// lab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _SILENCE_AMP_DEPRECATION_WARNINGS
#include <amp.h>
#include <iostream>
#include "timer.h"
#include <algorithm>

using namespace concurrency;
using namespace std;


void getInfo()
{
    auto accelerators = accelerator::get_all();

    for (const auto& device: accelerators)
    {
        wcout << "\n\nName: " << device.get_description() << '\n'
            << "Path: " << device.get_device_path() << '\n'
            << "Memory: " << device.dedicated_memory << '\n'
            << "Debug: " << (device.get_is_debug() ? "Yes" : "No") << '\n'
            << "Emulated: " << (device.get_is_emulated() ? "Yes" : "No") << '\n'
            << "Shared mem: " << (device.get_supports_cpu_shared_memory() ? "Yes" : "No") << '\n'
            << "Supp. limited double prec.: " << (device.get_supports_limited_double_precision() ? "Yes" : "No") << '\n'
            << "Supp. double prec.: " << (device.get_supports_double_precision() ? "Yes" : "No") << '\n'
            << "has display: " << (device.get_has_display() ? "Yes" : "No") << '\n';
    }
}

// Vector's sum

void sum_consequent(const int* arr1, const int* arr2, int array_size, int* out)
{
    for (auto i = 0; i < array_size; i++)
        out[i] = arr1[i] + arr2[i];
}

void sum_consequent_mp(const int* arr1, const int* arr2, int array_size, int* out)
{
    #pragma omp parallel for
    for (auto i = 0; i < array_size; i++)
        out[i] = arr1[i] + arr2[i];
}

void sum_concurrent_gpu(int* arr1, int* arr2, int array_size, int* out)
{
    const array_view<const int, 1> a1(array_size, arr1);
    const array_view<const int, 1> a2(array_size, arr2);
    const array_view<int, 1> res(array_size, out);
    res.discard_data();

    parallel_for_each(res.extent, [=](index<1> ind) restrict(amp)
        {
            res[ind] = a1[ind] + a2[ind];
        }
    );

    res.synchronize();
}

// Multiply

void mul_consequent(const int* arr1, int array_size, int* out)
{
    for (int i = 0; i < array_size * array_size; i++)
        out[i] = arr1[i] * 5;
}

void mul_consequent_mp(const int* arr1, int array_size, int* out)
{
    #pragma omp parallel for
    for (int i = 0; i < array_size * array_size; i++)
        out[i] = arr1[i] * 5;
}

void mul_concurrent_gpu(int* arr1, int array_size, int* out)
{
    const array_view<const int, 2> a1(array_size, array_size, arr1);
    const array_view<int, 2> res(array_size, array_size, out);
    res.discard_data();

    parallel_for_each(res.extent, [=](index<2> ind) restrict(amp)
        {
            res[ind] = a1[ind] * 5;
        }
    );

    res.synchronize();
}

// Tr.

void tr_consequent(const int* arr1, int array_size, int* out)
{
    for (int i = 0; i < array_size; i++)
        for (int j = 0; j < array_size; j++)
            out[i * array_size + j] = arr1[j * array_size + i];
}

void tr_consequent_mp(const int* arr1, int array_size, int* out)
{
    #pragma omp parallel for
    for (int i = 0; i < array_size; i++)
        for (int j = 0; j < array_size; j++)
            out[i * array_size + j] = arr1[j * array_size + i];
}

void tr_concurrent_gpu(int* arr1, int array_size, int* out)
{
    const array_view<const int, 2> a1(array_size, array_size, arr1);
    const array_view<int, 2> res(array_size, array_size, out);
    res.discard_data();

    parallel_for_each(res.extent, [=](index<2> ind) restrict(amp)
        {
            res(ind[0], ind[1]) = a1(ind[1], ind[0]);
        }
    );

    res.synchronize();
}

// m_mul.

void m_mul_consequent(const int* arr1, const int* arr2, int array_size, int* out)
{
    for (int i = 0; i < array_size; i++)
        for (int j = 0; j < array_size; j++)
        {
            int sum = 0;
            for (int k = 0; k < array_size; k++)
                sum += arr1[i * array_size + k] * arr2[k * array_size + j];
            out[i * array_size + j] = sum;
        }
}

void m_mul_consequent_mp(const int* arr1, const int* arr2, int array_size, int* out)
{
    #pragma omp parallel for
    for (int i = 0; i < array_size; i++)
        for (int j = 0; j < array_size; j++)
        {
            int sum = 0;
            for (int k = 0; k < array_size; k++)
                sum += arr1[i * array_size + k] * arr2[k * array_size + j];
            out[i * array_size + j] = sum;
        }
}

void m_mul_concurrent_gpu(int* arr1, int* arr2, int array_size, int* out)
{
    const array_view<const int, 2> a1(array_size, array_size, arr1);
    const array_view<const int, 2> a2(array_size, array_size, arr2);
    const array_view<int, 2> res(array_size, array_size, out);
    res.discard_data();

    parallel_for_each(res.extent, [=](index<2> ind) restrict(amp)
        {
            int sum = 0;
            for (int k = 0; k < array_size; k++)
                sum += a1(ind[0], k) * a2(k, ind[1]);
            res[ind] = sum;
        }
    );

    res.synchronize();
}


// RESULTS

void sum()
{
    wcout << "\nSERIAL\n\n";

    for (int i = 2; i < 9; i++)
    {
        Timer t;

        int size = pow(10, i);
        int* out = new int[size];
        int* array_a = new int[size];
        int* array_b = new int[size];
        fill(array_a, array_a + size, 10);
        fill(array_b, array_b + size, 20);

        t.Start();
        sum_consequent(array_a, array_b, size, out);
        t.Stop();
        if (i == 2) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nMP\n\n";

    for (int i = 2; i < 9; i++)
    {
        Timer t;

        int size = pow(10, i);
        int* out = new int[size];
        int* array_a = new int[size];
        int* array_b = new int[size];
        fill(array_a, array_a + size, 10);
        fill(array_b, array_b + size, 20);

        t.Start();
        sum_consequent_mp(array_a, array_b, size, out);
        t.Stop();
        if (i == 2) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU\n\n";
    for (int i = 2; i < 9; i++)
    {
        Timer t;

        int size = pow(10, i);
        int* out = new int[size];
        int* array_a = new int[size];
        int* array_b = new int[size];
        fill(array_a, array_a + size, 10);
        fill(array_b, array_b + size, 20);

        t.Start();
        sum_concurrent_gpu(array_a, array_b, size, out);
        t.Stop();
        if (i == 2) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }
}

void mul()
{
    int* sizes = new int[] { 1, 100, 1000, 5000, 10000, 20000 };

    wcout << "\nSERIAL\n\n";

    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 10);

        t.Start();
        mul_consequent(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nMP\n\n";

    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 10);

        t.Start();
        mul_consequent_mp(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU\n\n";
    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 10);

        t.Start();
        mul_concurrent_gpu(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }
}

void tr()
{
    int* sizes = new int[] { 1, 100, 1000, 5000, 10000, 20000 };

    wcout << "\nSERIAL\n\n";

    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 10);

        t.Start();
        tr_consequent(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nMP\n\n";

    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 10);

        t.Start();
        tr_consequent_mp(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU\n\n";
    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 10);

        t.Start();
        tr_concurrent_gpu(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }
}

void m_mul()
{
    int* sizes = new int[] { 1, 100, 500, 1000, 2000 };

    wcout << "\nSERIAL\n\n";

    for (int i = 0; i < 5; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        int* array_b = new int[size * size];
        fill(array_a, array_a + size * size, 10);
        fill(array_b, array_b + size * size, 20);

        t.Start();
        m_mul_consequent(array_a, array_b, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nMP\n\n";

    for (int i = 0; i < 5; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        int* array_b = new int[size * size];
        fill(array_a, array_a + size * size, 10);
        fill(array_b, array_b + size * size, 20);

        t.Start();
        m_mul_consequent_mp(array_a, array_b, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU\n\n";
    for (int i = 0; i < 5; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        int* array_b = new int[size * size];
        fill(array_a, array_a + size * size, 10);
        fill(array_b, array_b + size * size, 20);

        t.Start();
        m_mul_concurrent_gpu(array_a, array_b, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }
}

int main()
{
    m_mul();
}