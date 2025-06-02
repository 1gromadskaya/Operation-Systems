#include <iostream>
#include <cassert>
#include "../lab_functions.h"

void test_computeMinMax() {
    int arr[] = {5, 2, 8, 1, 3};
    int min, max;
    computeMinMax(arr, 5, min, max);
    assert(min == 1);
    assert(max == 8);
    std::cout << "test_computeMinMax passed." << std::endl;
}

void test_computeAverage() {
    int arr[] = {5, 2, 8, 1, 4}; // сумма = 20, среднее = 4.0
    double avg = computeAverage(arr, 5);
    assert(avg == 4.0);
    std::cout << "test_computeAverage passed." << std::endl;
}

int main()
{
    test_computeMinMax();
    test_computeAverage();
    std::cout << "All tests passed." << std::endl;
    return 0;
}