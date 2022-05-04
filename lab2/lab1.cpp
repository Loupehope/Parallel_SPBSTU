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
#include <amp_graphics.h>

using namespace concurrency;
using namespace std;
using namespace concurrency::graphics;

// Tr.


void display_board(float* board, int size)
{
    for (int i = 0; i < size + 2; i++)
    {
        for (int j = 0; j < size + 2; j++)
        {
            if (board[i * (size + 2) + j])
                cout << "|" << board[i * (size + 2) + j] << "|";
            else
                cout << "|_|";
        }
        cout << endl;
    }
    cout << endl;
}

float* serial_new_generation(float* board, int size, float* warmers, int warmers_count, float k)
{
    int offset_size = size + 2;
    float neighbours;
    float* temp_board = new float[offset_size * offset_size];
    fill(temp_board, temp_board + (size + 2) * (size + 2), 0);

    for (int i = 1; i < size + 1; i++)
    {
        for (int j = 1; j < size + 1; j++)
        {
            int idx = (i * offset_size) + j;
            float num_k = 4;

            neighbours = board[(i - 1) * offset_size + j];
            neighbours += board[i * offset_size + (j - 1)];
            neighbours += board[i * offset_size + (j + 1)];
            neighbours += board[(i + 1) * offset_size + j];

            temp_board[idx] = board[idx] + k * (neighbours - num_k * board[idx]);
        }
    }

    for (int i = 0; i < warmers_count; i++) {
        int x = warmers[i * 3];
        int y = warmers[i * 3 + 1];
        float value = warmers[i * 3 + 2];

        temp_board[y * (size + 2) + x] = value;
    }

    delete[] board;

    return temp_board;
}

float* gpu_new_generation(float* board, int size, float* warmers, int warmers_count, float k)
{
    int offset_size = size + 2;
    int full_size = offset_size * offset_size;
    float* temp_board = new float[full_size];
    fill(temp_board, temp_board + full_size, 0);

    array_view<const float, 2> input(offset_size, offset_size, board);
    array_view<float, 2> output(offset_size, offset_size, temp_board);
    output.discard_data();

    concurrency::extent<2> write(size, size);

    parallel_for_each(write, [=](index<2> idx) restrict(amp)
        {
            float neighbours = 0;
            float num_k = 4;

            neighbours += input(idx[0], idx[1] + 1);
            neighbours += input(idx[0] + 1, idx[1]);
            neighbours += input(idx[0] + 1, idx[1] + 2);
            neighbours += input(idx[0] + 2, idx[1] + 1);
            float value = input(idx[0] + 1, idx[1] + 1);

            output(idx[0] + 1, idx[1] + 1) = value + k * (neighbours - num_k * value);
        }
    );

    output.synchronize();

    for (int i = 0; i < warmers_count; i++) {
        int x = warmers[i * 3];
        int y = warmers[i * 3 + 1];
        float value = warmers[i * 3 + 2];

        temp_board[y * (size + 2) + x] = value;
    }

    delete[] board;

    return temp_board;
}

float* texture_new_generation(float* board, int size, float* warmers, int warmers_count, float k)
{
    int offset_size = size + 2;
    int full_size = offset_size * offset_size;
    float* temp_board = new float[full_size];
    fill(temp_board, temp_board + full_size, 0);

    texture<float, 2> input_texture(offset_size, offset_size, board, board + full_size);
    texture_view<const float, 2> input(input_texture);
    array_view<float, 2> output(offset_size, offset_size, temp_board);
    output.discard_data();

    concurrency::extent<2> write(size, size);

    parallel_for_each(write, [=](index<2> idx) restrict(amp)
        {
            float neighbours = 0;
            float num_k = 4;

            neighbours += input(idx[0], idx[1] + 1);
            neighbours += input(idx[0] + 1, idx[1]);
            neighbours += input(idx[0] + 1, idx[1] + 2);
            neighbours += input(idx[0] + 2, idx[1] + 1);
            float value = input(idx[0] + 1, idx[1] + 1);

            output(idx[0] + 1, idx[1] + 1) = value + k * (neighbours - num_k * value);
        }
    );

    output.synchronize();

    for (int i = 0; i < warmers_count; i++) {
        int x = warmers[i * 3];
        int y = warmers[i * 3 + 1];
        float value = warmers[i * 3 + 2];

        temp_board[y * (size + 2) + x] = value;
    }

    delete[] board;

    return temp_board;
}

// RESULTS


double tr_1(float* life, int size, float* warmers, int warmers_count, float k) {
    Timer t;
    float* input = new float[(size + 2) * (size + 2)];
    copy(life, life + (size + 2) * (size + 2), input);

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = serial_new_generation(input, size, warmers, warmers_count, k);
        }
        t.Stop();
        if (i == 0) continue;
    }

    return t.Elapsed();
}

double tr2(float* life, int size, float* warmers, int warmers_count, float k) {
    Timer t;
    float* input = new float[(size + 2) * (size + 2)];
    copy(life, life + (size + 2) * (size + 2), input);

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = gpu_new_generation(input, size, warmers, warmers_count, k);
        }
        t.Stop();
        if (i == 0) continue;
    }

    return t.Elapsed();
}

double tr3(float* life, int size, float* warmers, int warmers_count, float k) {
    Timer t;
    float* input = new float[(size + 2) * (size + 2)];
    copy(life, life + (size + 2) * (size + 2), input);

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = texture_new_generation(input, size, warmers, warmers_count, k);
        }
        t.Stop();
        if (i == 0) continue;
    }

    return t.Elapsed();
}

int main()
{
    srand(time(0));

    double times[5][5] = {{0, 0, 0, 0, 0}, { 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0 }};

    int* sizes = new int[] { 128, 256, 512, 1024, 2048 };

    for (int i = 0; i < 5; i++)
    {
        int size = sizes[i];
        float* life = new float[(size + 2) * (size + 2)];
        fill(life, life + (size + 2) * (size + 2), 0);
        int max_warmer_count = 100;
        int warmer_count = 0;
        float k = 0.3;
        float* warmers = new float[max_warmer_count * 3];

        for (int i = 0; i < max_warmer_count; i++) {
            int x = rand() % size + 1;
            int y = rand() % size + 1;

            if (life[y * (size + 2) + x] == 0) {
                life[y * (size + 2) + x] = rand() % 100 + 20;
                warmers[warmer_count * 3] = x;
                warmers[warmer_count * 3 + 1] = y;
                warmers[warmer_count * 3 + 2] = life[y * (size + 2) + x];
                warmer_count++;
            }
        }

        times[0][i] = tr_1(life, size, warmers, warmer_count, k);
        times[1][i] = tr2(life, size, warmers, warmer_count, k);
        times[2][i] = tr3(life, size, warmers, warmer_count, k);
        // times[3][i] = tr4(life, size);
        // times[4][i] = tr5(life, size);

        wcout << i << '\n';
    }
    wcout << '\n';

    for (int i = 0; i < 5; i++) {
        wcout << "Mehod: " << i << '\n';

        for (int j = 0; j < 5; j++) {
            wcout << "Time: " << times[i][j] << " Count: " << sizes[j] << '\n';
        }

        wcout << '\n';
    }
}