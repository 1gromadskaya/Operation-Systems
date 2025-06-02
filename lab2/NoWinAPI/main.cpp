#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "globals.h"
#include "lab_functions.h"

using namespace std;

int main()
{
    int size;
    cout << "Enter the number of elements in the array: ";
    cin >> size;

    if (size <= 0)
    {
        cout << "Invalid array size!" << endl;
        return 1;
    }

    g_array.resize(size);
    cout << "Enter " << size << " integers:" << endl;
    for (int i = 0; i < size; i++)
    {
        cin >> g_array[i];
    }

    thread t1(MinMaxThread);
    thread t2(AverageThread);

    t1.join();
    t2.join();

    for (int& val : g_array)
    {
        if (val == g_min || val == g_max)
        {
            val = static_cast<int>(g_avg);
        }
    }

    cout << "Modified array: ";
    for (int val : g_array)
    {
        cout << val << " ";
    }
    cout << endl;

    return 0;
}
