#include "lab_functions.h"
#include <windows.h>

void computeMinMax(const int* array, int size, int &min, int &max)
{
    if (size <= 0)
        return;
    
    min = array[0];
    max = array[0];
    for (int i = 1; i < size; i++)
    {
        if (array[i] < min)
        {
            min = array[i];
        }
        Sleep(7); // пауза 7 мс после сравнения
        
        if (array[i] > max)
        {
            max = array[i];
        }
        Sleep(7); // пауза 7 мс после сравнения
    }
}

double computeAverage(const int* array, int size)
{
    if (size <= 0)
        return 0;
    
    long long sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += array[i];
        Sleep(12); // пауза 12 мс после каждой операции суммирования
    }
    return static_cast<double>(sum) / size;
}