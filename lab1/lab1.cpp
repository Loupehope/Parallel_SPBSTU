// lab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _SILENCE_AMP_DEPRECATION_WARNINGS
#include <amp.h>
#include <iostream>
#include "timer.h"
#include <algorithm>

using namespace concurrency;
using namespace std;

// Tr.

static const int tile_size_x = 32;


void tr_consequent(const int* arr1, int array_size, int* out)
{
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

void tr_concurrent_gpu_block(int* arr1, int array_size, int* out)
{
    const array_view<const int, 2> in_data(array_size, array_size, arr1);
    const array_view<int, 2> out_data(array_size, array_size, out);
    out_data.discard_data();

    parallel_for_each(out_data.extent.tile<tile_size_x, tile_size_x>(), [=](tiled_index<tile_size_x, tile_size_x> tidx) restrict(amp) {
        tile_static int local_data[tile_size_x][tile_size_x];

        local_data[tidx.local[1]][tidx.local[0]] = in_data[tidx.global];

        tidx.barrier.wait();

        const auto out_idx(index<2>(tidx.tile_origin[1], tidx.tile_origin[0]) + tidx.local);

        out_data[out_idx] = local_data[tidx.local[0]][tidx.local[1]];
    });

    out_data.synchronize();
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

void m_mul_concurrent_gpu_block(int* arr1, int* arr2, int array_size, int* out)
{
    const array_view<const int, 2> a1(array_size, array_size, arr1);
    const array_view<const int, 2> a2(array_size, array_size, arr2);
    const array_view<int, 2> out_data(array_size, array_size, out);
    out_data.discard_data();

    parallel_for_each(out_data.extent.tile<tile_size_x, tile_size_x>(), [=](tiled_index<tile_size_x, tile_size_x> tidx) restrict(amp) {
        auto row = tidx.global[0];
        auto col = tidx.global[1];
        auto sum = 0;

        for (auto i = 0; i < array_size; i++)
            sum += a1(row, i) * a2(i, col);

        out_data[tidx.global] = sum;
      });

    out_data.synchronize();
}

void m_mul_concurrent_gpu_static(int* arr1, int* arr2, int array_size, int* out)
{
    const array_view<const int, 2> a1(array_size, array_size, arr1);
    const array_view<const int, 2> a2(array_size, array_size, arr2);
    const array_view<int, 2> out_data(array_size, array_size, out);
    out_data.discard_data();

    parallel_for_each(out_data.extent.tile<tile_size_x, tile_size_x>(), [=](tiled_index<tile_size_x, tile_size_x> tidx) restrict(amp) {
        auto row = tidx.local[0];
        auto col = tidx.local[1];
        auto sum = 0;

        for (auto i = 0; i < array_size; i += tile_size_x) {
            tile_static int sA[tile_size_x][tile_size_x];
            tile_static int sB[tile_size_x][tile_size_x];

            sA[row][col] = a1(tidx.global[0], i + col);
            sB[row][col] = a2(i + row, tidx.global[1]);

            tidx.barrier.wait();
            
            for (auto k = 0; k < tile_size_x; k++)
                sum += sA[row][k] * sB[k][col];

            tidx.barrier.wait();
        }
 
        out_data[tidx.global] = sum;
     });

    out_data.synchronize();
}

void m_mul_concurrent_gpu_big(int* arr1, int* arr2, int array_size, int* out)
{
    const array_view<const int, 2> a1(array_size, array_size, arr1);
    const array_view<const int, 2> a2(array_size, array_size, arr2);
    const array_view<int, 2> out_data(array_size, array_size, out);
    out_data.discard_data();

    concurrency::extent<1> ext(array_size);

    parallel_for_each(ext, [=](index<1> tidx) restrict(amp) {
        const auto row = tidx[0];
        for (auto j = 0; j < array_size; j++) {
            auto sum = 0;
            for (auto i = 0; i < array_size; i++)
                sum += a1(row, i) * a2(i, j);
            out_data(row, j) = sum;
        }
        });

    out_data.synchronize();
}

// RESULTS

void tr()
{
    int* sizes = new int[] { 128, 128, 512, 2048, 4096, 8192 };

    wcout << "\nSERIAL\n\n";

    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 2);

        t.Start();
        tr_consequent(array_a, size, out);
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
        fill(array_a, array_a + size * size, 2);

        t.Start();
        tr_concurrent_gpu(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU - BLOCK \n\n";
    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* out = new int[size * size];
        int* array_a = new int[size * size];
        fill(array_a, array_a + size * size, 2);

        t.Start();
        tr_concurrent_gpu_block(array_a, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }
}

void m_mul()
{
    int* sizes = new int[] { 128, 128, 256, 512, 2048 };

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

    wcout << "\nGPU BLOCK\n\n";
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
        m_mul_concurrent_gpu_block(array_a, array_b, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU STATIC\n\n";
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
        m_mul_concurrent_gpu_static(array_a, array_b, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU BIG\n\n";
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
        m_mul_concurrent_gpu_big(array_a, array_b, size, out);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }
}

int main()
{
    m_mul();
}