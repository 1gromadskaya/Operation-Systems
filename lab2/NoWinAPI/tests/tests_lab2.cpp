#include <cassert>
#include <vector>
#include <thread>
#include <iostream>
#include "../globals.h"
#include "../lab_functions.h"

void testMinMax()
{
    std::vector<int> arr = {5, 1, 9, 3, 7};
    int min = 0, max = 0;
    computeMinMax(arr, min, max);
    assert(min == 1);
    assert(max == 9);
}

void testAverage()
{
    std::vector<int> arr = {2, 4, 6, 8};
    double avg = computeAverage(arr);
    assert(avg == 5.0);
}

void testIntegration()
{
    g_array = {1, 3, 5, 7, 9};

    std::thread t1(MinMaxThread);
    std::thread t2(AverageThread);
    t1.join();
    t2.join();

    for (int& val : g_array)
    {
        if (val == g_min || val == g_max)
            val = static_cast<int>(g_avg);
    }

    assert(g_min == 1);
    assert(g_max == 9);
    assert(g_avg == 5.0);

    std::vector<int> expected = {5, 3, 5, 7, 5};
    assert(g_array == expected);
}

int main()
{
    testMinMax();
    testAverage();
    testIntegration();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}