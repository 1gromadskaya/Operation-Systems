#include "lab_functions.h"
#include <chrono>
#include <thread>
#include <iostream>

void computeMinMax(const std::vector<int>& array, int& min, int& max)
{
    if (array.empty())
        return;

    min = max = array[0];

    for (size_t i = 1; i < array.size(); ++i)
    {
        if (array[i] < min)
        {
            min = array[i];
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(7));

        if (array[i] > max)
        {
            max = array[i];
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }
}

double computeAverage(const std::vector<int>& array)
{
    if (array.empty())
        return 0.0;

    long long sum = 0;
    for (int val : array)
    {
        sum += val;
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }

    return static_cast<double>(sum) / array.size();
}

void MinMaxThread()
{
    computeMinMax(g_array, g_min, g_max);
    std::cout << "Minimum: " << g_min << ", Maximum: " << g_max << std::endl;
}

void AverageThread()
{
    g_avg = computeAverage(g_array);
    std::cout << "Average value: " << g_avg << std::endl;
}
