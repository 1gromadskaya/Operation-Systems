#ifndef LAB_FUNCTIONS_H
#define LAB_FUNCTIONS_H

#include <vector>
#include "globals.h"

void computeMinMax(const std::vector<int>& array, int& min, int& max);

double computeAverage(const std::vector<int>& array);

void MinMaxThread();

void AverageThread();

#endif // LAB_FUNCTIONS_H
