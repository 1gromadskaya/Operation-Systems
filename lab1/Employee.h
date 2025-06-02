#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

struct Employee {
    int num;         // id
    char name[10];   // имя
    double hours;    // часы
};

// запись в бинарный файл
inline bool writeEmployeeRecords(const char* filename, const std::vector<Employee>& employees)
{
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) return false;
    for (size_t i = 0; i < employees.size(); i++) {
        ofs.write(reinterpret_cast<const char*>(&employees[i]), sizeof(Employee));
    }
    return ofs.good();
}

// чтение из бинарного файлы
inline bool readEmployeeRecords(const char* filename, std::vector<Employee>& employees)
{
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) return false;
    Employee emp;
    while (ifs.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        employees.push_back(emp);
    }
    return ifs.eof();
}

// компаратор для сортировки по id по возрастанию
struct EmployeeComparator {
    bool operator()(const Employee &a, const Employee &b) const {
        return a.num < b.num;
    }
};

// сортировка с компаратором ^
inline void sortEmployees(std::vector<Employee>& employees)
{
    std::sort(employees.begin(), employees.end(), EmployeeComparator());
}

#endif // EMPLOYEE_H