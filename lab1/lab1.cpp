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

static const int tile_size = 32;

void display_board(int* board, int size)
{
    for (int i = 0; i < size + 2; i++)
    {
        for (int j = 0; j < size + 2; j++)
        {
            if (board[i * (size + 2) + j] == 1)
                cout << "|*|";
            else
                cout << "|_|";
        }
        cout << endl;
    }
    cout << endl;
}

void setup_hidden(int* board, int size)
{
    int offset_size = size + 2;
    int full_size = offset_size * offset_size;

    for (int i = 0; i < size; i++) {
        // top
        board[i + 1] = board[full_size - offset_size - offset_size + 1 + i];
        // bottom
        board[full_size - offset_size + 1 + i] = board[offset_size + 1 + i];
        // left
        board[(i + 1) * offset_size] = board[(i + 1) * offset_size + size];
        // right
        board[(i + 1) * offset_size + offset_size - 1] = board[(i + 1) * offset_size + 1];
    }

    board[0] = board[full_size - offset_size - 2];
    board[offset_size - 1] = board[full_size - offset_size - offset_size + 1];
    board[full_size - offset_size] = board[offset_size + size];
    board[full_size - 1] = board[offset_size + 1];
}

int* serial_new_generation(int* board, int size)
{
    int* temp_board = new int[(size + 2) * (size + 2)];
    int offset_size = size + 2;
    int neighbours;

    for (int i = 1; i < size + 1; i++)
    {
        for (int j = 1; j < size + 1; j++)
        {
            int idx = (i * offset_size) + j;

            neighbours = board[(i - 1) * offset_size + (j - 1)];
            neighbours += board[(i - 1) * offset_size + j];
            neighbours += board[(i - 1) * offset_size + (j + 1)];
            neighbours += board[i * offset_size + (j - 1)];
            neighbours += board[i * offset_size + (j + 1)];
            neighbours += board[(i + 1) * offset_size + (j - 1)];
            neighbours += board[(i + 1) * offset_size + j];
            neighbours += board[(i + 1) * offset_size + (j + 1)];

            if (board[idx] == 1)
            {
                if (neighbours < 2 || neighbours > 3)
                    temp_board[idx] = 0;
                else
                    temp_board[idx] = 1;
            }

            else
            {
                if (neighbours == 3)
                    temp_board[idx] = 1;
                else
                    temp_board[idx] = 0;
            }
        }
    }

    delete[] board;
    setup_hidden(temp_board, size);

    return temp_board;
}

int* gpu_new_generation(int* board, int size)
{
    int offset_size = size + 2;
    int full_size = offset_size * offset_size;
    int* temp_board = new int[full_size];

    array_view<const int, 2> input(offset_size, offset_size, board);
    array_view<int, 2> output(offset_size, offset_size, temp_board);
    output.discard_data();

    concurrency::extent<2> write(size, size);

    parallel_for_each(write, [=](index<2> idx) restrict(amp)
        {
            int neighbours = 0;

            neighbours = input(idx[0], idx[1]);
            neighbours += input(idx[0], idx[1] + 1);
            neighbours += input(idx[0], idx[1] + 2);
            neighbours += input(idx[0] + 1, idx[1]);
            neighbours += input(idx[0] + 1, idx[1] + 2);
            neighbours += input(idx[0] + 2, idx[1]);
            neighbours += input(idx[0] + 2, idx[1] + 1);
            neighbours += input(idx[0] + 2, idx[1] + 2);

            if (neighbours < 2 || neighbours > 3)
            {
                output(idx[0] + 1, idx[1] + 1) = 0;
            } 
            else if (neighbours == 3)
            {
                output(idx[0] + 1, idx[1] + 1) = 1;
            }
            else
            {
                output(idx[0] + 1, idx[1] + 1) = input(idx[0] + 1, idx[1] + 1);
            }
        }
    );
    output.synchronize();

    delete[] board;
    setup_hidden(temp_board, size);

    return temp_board;
}

int* optimized_gpu_new_generation(int* board, int size)
{
    int offset_size = size + 2;
    int full_size = offset_size * offset_size;
    int* temp_board = new int[offset_size * offset_size];
    int* rules = new int[] { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 };

    array_view<const int, 2> input(offset_size, offset_size, board);
    array_view<const int, 2> rules_in(2, 9, rules);
    array_view<int, 2> output(offset_size, offset_size, temp_board);
    output.discard_data();

    concurrency::extent<2> write(size, size);

    parallel_for_each(write, [=](index<2> idx) restrict(amp)
        {
            int neighbours = 0;

            neighbours = input(idx[0], idx[1]);
            neighbours += input(idx[0], idx[1] + 1);
            neighbours += input(idx[0], idx[1] + 2);
            neighbours += input(idx[0] + 1, idx[1]);
            neighbours += input(idx[0] + 1, idx[1] + 2);
            neighbours += input(idx[0] + 2, idx[1]);
            neighbours += input(idx[0] + 2, idx[1] + 1);
            neighbours += input(idx[0] + 2, idx[1] + 2);

            int alive = input(idx[0] + 1, idx[1] + 1);

            output(idx[0] + 1, idx[1] + 1) = rules_in(alive, neighbours);
        }
    );
    output.synchronize();

    delete[] board;
    setup_hidden(temp_board, size);


    return temp_board;
}

int* const_gpu_new_generation(int* board, int size)
{
    int offset_size = size + 2;
    int full_size = offset_size * offset_size;
    int* temp_board = new int[offset_size * offset_size];
    int rules[2][9] = { {0, 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0, 0 } };

    array_view<const int, 2> input(offset_size, offset_size, board);
    array_view<int, 2> output(offset_size, offset_size, temp_board);
    output.discard_data();

    concurrency::extent<2> write(size, size);

    parallel_for_each(write, [=](index<2> idx) restrict(amp)
        {
            int neighbours = 0;

            neighbours = input(idx[0], idx[1]);
            neighbours += input(idx[0], idx[1] + 1);
            neighbours += input(idx[0], idx[1] + 2);
            neighbours += input(idx[0] + 1, idx[1]);
            neighbours += input(idx[0] + 1, idx[1] + 2);
            neighbours += input(idx[0] + 2, idx[1]);
            neighbours += input(idx[0] + 2, idx[1] + 1);
            neighbours += input(idx[0] + 2, idx[1] + 2);

            int alive = input(idx[0] + 1, idx[1] + 1);

            output(idx[0] + 1, idx[1] + 1) = rules[alive][neighbours];
        }
    );
    output.synchronize();

    delete[] board;
    setup_hidden(temp_board, size);


    return temp_board;
}

int* tile_gpu_new_generation(int* board, int size)
{
    int offset_size = size + 2;
    int full_size = offset_size * offset_size;
    int* temp_board = new int[offset_size * offset_size];
    int rules[2][9] = { {0, 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0, 0 } };

    array_view<const int, 2> input(offset_size, offset_size, board);
    array_view<int, 2> output(offset_size, offset_size, temp_board);
    output.discard_data();

    concurrency::extent<2> write(size, size);

    parallel_for_each(write.tile<tile_size, tile_size>(), [=](tiled_index<tile_size, tile_size> idx) restrict(amp)
        {
            tile_static int tileData[2][9];
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 9; j++) {
                    tileData[i][j] = rules[i][j];
                }
            }

            idx.barrier.wait();

            int neighbours = 0;

            neighbours = input(idx.global[0], idx.global[1]);
            neighbours += input(idx.global[0], idx.global[1] + 1);
            neighbours += input(idx.global[0], idx.global[1] + 2);
            neighbours += input(idx.global[0] + 1, idx.global[1]);
            neighbours += input(idx.global[0] + 1, idx.global[1] + 2);
            neighbours += input(idx.global[0] + 2, idx.global[1]);
            neighbours += input(idx.global[0] + 2, idx.global[1] + 1);
            neighbours += input(idx.global[0] + 2, idx.global[1] + 2);

            int alive = input(idx.global[0] + 1, idx.global[1] + 1);

            output(idx.global[0] + 1, idx.global[1] + 1) = tileData[alive][neighbours];
        }
    );
    output.synchronize();

    delete[] board;
    setup_hidden(temp_board, size);

    return temp_board;
}

// RESULTS


double tr_1(int* life, int size) {
    Timer t;
    int* input = new int[(size + 2) * (size + 2)];
    memcpy(input, life, (size + 2) * (size + 2));

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = serial_new_generation(input, size);
        }
        t.Stop();
        if (i == 0) continue;
    }
    
    return t.Elapsed();
}

double tr2(int* life, int size) {
    Timer t;
    int* input = new int[(size + 2) * (size + 2)];
    copy(life, life + (size + 2) * (size + 2), input);

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = gpu_new_generation(input, size);
        }
        t.Stop();
        if (i == 0) continue;
    }

    return t.Elapsed();
}
double tr3(int* life, int size) {
    Timer t;
    int* input = new int[(size + 2) * (size + 2)];
    copy(life, life + (size + 2) * (size + 2), input);

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = optimized_gpu_new_generation(input, size);
        }
        t.Stop();
        if (i == 0) continue;
    }

    return t.Elapsed();
}
double tr4(int* life, int size) {
    Timer t;
    int* input = new int[(size + 2) * (size + 2)];
    copy(life, life + (size + 2) * (size + 2), input);

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = const_gpu_new_generation(input, size);
        }
        t.Stop();
        if (i == 0) continue;
    }

    return t.Elapsed();
}
double tr5(int* life, int size) {
    Timer t;
    int* input = new int[(size + 2) * (size + 2)];
    copy(life, life + (size + 2) * (size + 2), input);

    for (int i = 0; i < 2; i++) {
        t.Start();
        for (int i = 0; i < 1024; i++) {
            input = tile_gpu_new_generation(input, size);
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
        int* life = new int[(size + 2) * (size + 2)];

        fill(life, life + (size + 2) * (size + 2), 0);

        for (int i = 1; i < size + 1; i++) {
            for (int j = 1; j < size + 1; j++) {
                life[i * (size + 2) + j] = rand() % 2;
            }
        }

        setup_hidden(life, size);

        times[0][i] = tr_1(life, size);   
        times[1][i] = tr2(life, size);
        times[2][i] = tr3(life, size);
        times[3][i] = tr4(life, size);
        times[4][i] = tr5(life, size);

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