// lab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _SILENCE_AMP_DEPRECATION_WARNINGS
#include <amp.h>
#include <iostream>
#include "timer.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <functional>

using namespace concurrency;
using namespace std;

// Tr.

static const int tile_size = 1024;


void tr_consequent(int* arr1, int array_size)
{
    for (int i = 1; i < array_size; i++)
        arr1[0] += arr1[1];
}

void tr_concurrent_gpu(int* arr1, int array_size)
{
    int elementCount = static_cast<int>(array_size);
    int last_stride = array_size;

    array_view<int, 1> result(array_size, arr1);

    for (int stride = elementCount / 2; stride > 0; stride /= 2)
    {
        concurrency::extent<1> ext(stride);

        parallel_for_each(ext, [=](index<1> ind) restrict(amp)
            {
                result[ind] += result[ind + stride];

                if ((ind[0] == 0) && (last_stride % 2) && (stride > 1))
                    result[ind] += result[ind + last_stride - 1];
            });

        last_stride = stride;
    }

    result.synchronize();
}

void tr_concurrent_gpu_block(int* arr1, int array_size, int windowWidth)
{
    int elementCount = static_cast<int>(array_size);
    int last_stride = array_size;

    array_view<int, 1> result(array_size, arr1);

    for (int stride = (elementCount / windowWidth); stride > 0; stride /= windowWidth)
    {
        concurrency::extent<1> ext(stride);

        parallel_for_each(ext, [=](index<1> ind) restrict(amp)
            {
                int sum = 0;
                for (int i = 0; i < windowWidth; i++)
                    sum += result[ind + i * stride];

                result[ind] = sum;

                if ((ind[0] == (stride - 1)) && ((last_stride % windowWidth) != 0))
                {
                    sum = 0;
                    for (int i = stride * windowWidth; i < last_stride; i++)
                        sum += result[ind + i];

                    result[ind] += sum;
                }
            });

        last_stride = stride;
    }

    result.synchronize();

    for (int i = 1; i < last_stride; i++)
        arr1[0] += arr1[1];
}

void ras_tr_concurrent_gpu_block(int* arr1, int array_size, int windowWidth)
{
    int elementCount = static_cast<int>(array_size);
    array_view<int, 1> result(elementCount, arr1);

    while (elementCount >= tile_size)
    {
        concurrency::extent<1> ext(elementCount);

        parallel_for_each(ext.tile<tile_size>(), [=](tiled_index<tile_size> ind) restrict(amp)
            {
                int tid = ind.local[0];
                tile_static int tileData[tile_size];

                tileData[tid] = result[ind.global[0]];

                ind.barrier.wait();

                for (int stride = 1; stride < tile_size; stride *= 2)
                {
                    if (tid % (2 * stride) == 0)
                        tileData[tid] += tileData[tid + stride];

                    ind.barrier.wait();
                }

                if (tid == 0)
                    result[ind.tile[0]] = tileData[0];
            });

        elementCount /= tile_size;
    }

    result.synchronize();

    for (int i = 1; i < elementCount; i++)
        arr1[0] += arr1[1];
}

void p_ras_tr_concurrent_gpu_block(int* arr1, int array_size, int windowWidth)
{
    int elementCount = static_cast<int>(array_size);
    array_view<int, 1> result(elementCount, arr1);
    int window = elementCount / windowWidth;

    concurrency::extent<1> ext(window);

    parallel_for_each(ext.tile<tile_size>(), [=](tiled_index<tile_size> ind) restrict(amp)
        {

            int tid = ind.local[0];
            tile_static int tileData[tile_size];
            int i = ind.global[0];
            int stride = window * 2;

            int sum = 0;
            do
            {
                sum += result[i] + result[i + window];
                i += stride;
            } while (i < elementCount);

            tileData[tid] = sum;

            ind.barrier.wait();

            for (int stride = (tile_size / 2); stride > 0; stride >>= 1)
            {
                if (tid < stride) tileData[tid] += tileData[tid + stride];

                ind.barrier.wait();
            }

            if (tid == 0)
                result[ind.tile[0]] = tileData[0];
        });

    result.synchronize();

    for (int i = 1; i < (window / tile_size); i++)
        arr1[0] += arr1[1];
}

// RESULTS

void tr()
{
    int* sizes = new int[] { 1048576, 1048576, 2097152, 8388608, 16777216, 33554432 };

    wcout << "\nSERIAL\n\n";

    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* array_a = new int[size];
        fill(array_a, array_a + size, 2);

        t.Start();
        tr_consequent(array_a, size);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU\n\n";
    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* array_a = new int[size];
        fill(array_a, array_a + size, 2);

        t.Start();
        tr_concurrent_gpu(array_a, size);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU - BLOCK \n\n";
    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* array_a = new int[size];
        fill(array_a, array_a + size, 2);

        t.Start();
        ras_tr_concurrent_gpu_block(array_a, size, 64);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU - RAS BLOCK \n\n";
    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* array_a = new int[size];
        fill(array_a, array_a + size, 2);

        t.Start();
        ras_tr_concurrent_gpu_block(array_a, size, 64);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }

    wcout << "\nGPU - P RAS BLOCK \n\n";
    for (int i = 0; i < 6; i++)
    {
        Timer t;

        int size = sizes[i];
        int* array_a = new int[size];
        fill(array_a, array_a + size, 2);

        t.Start();
        p_ras_tr_concurrent_gpu_block(array_a, size, 128);
        t.Stop();
        if (i == 0) continue;

        wcout << "Time: " << t.Elapsed() << "\nCount: " << size << '\n';
    }
}

int main()
{
    tr();
}