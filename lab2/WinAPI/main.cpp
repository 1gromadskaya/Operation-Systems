#include <iostream>
#include <windows.h>
#include "lab_functions.h"

using namespace std;

// Global variables for data exchange between threads
int* g_array = NULL;
int g_size = 0;
int g_min = 0, g_max = 0;
double g_avg = 0.0;

// Thread for finding the minimum and maximum elements
DWORD WINAPI MinMaxThread(LPVOID lpParam)
{
    computeMinMax(g_array, g_size, g_min, g_max);
    cout << "Minimum: " << g_min << ", Maximum: " << g_max << endl;
    return 0;
}

// Thread for computing the average value
DWORD WINAPI AverageThread(LPVOID lpParam)
{
    g_avg = computeAverage(g_array, g_size);
    cout << "Average value: " << g_avg << endl;
    return 0;
}

int main()
{
    cout << "Enter the number of elements in the array: ";
    cin >> g_size;
    if (g_size <= 0)
    {
        cout << "Invalid array size!" << endl;
        return 1;
    }
    
    g_array = new int[g_size];
    cout << "Enter " << g_size << " integers:" << endl;
    for (int i = 0; i < g_size; i++)
    {
        cin >> g_array[i];
    }
    
    // Create threads for min/max search and average calculation
    DWORD threadIdMinMax, threadIdAverage;
    HANDLE hMinMax = CreateThread(NULL, 0, MinMaxThread, NULL, 0, &threadIdMinMax);
    HANDLE hAverage = CreateThread(NULL, 0, AverageThread, NULL, 0, &threadIdAverage);
    
    // Wait for threads to complete
    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);
    
    // Replace the minimum and maximum elements in the array with the average value
    for (int i = 0; i < g_size; i++)
    {
        if (g_array[i] == g_min || g_array[i] == g_max)
        {
            g_array[i] = static_cast<int>(g_avg);
        }
    }
    
    // Output the modified array
    cout << "Modified array: ";
    for (int i = 0; i < g_size; i++)
    {
        cout << g_array[i] << " ";
    }
    cout << endl;
    
    // Free resources
    CloseHandle(hMinMax);
    CloseHandle(hAverage);
    delete [] g_array;
    
    return 0;
}