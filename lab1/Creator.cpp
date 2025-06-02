#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include "Employee.h"

// Утилита Creator получает через командную строку:
// argv[1] – имя бинарного файла для записи,
// argv[2] – количество записей для вводаq.
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: Creator <binary_file_name> <number_of_records>" << std::endl;
        return 1;
    }

    const char* filename = argv[1];
    int numRecords = std::atoi(argv[2]);
    if(numRecords <= 0) {
        std::cerr << "Number of records must be positive." << std::endl;
        return 1;
    }

    std::vector<Employee> employees;
    for (int i = 0; i < numRecords; i++) {
        Employee emp;
        bool valid = false;
        while (!valid) {
            std::cout << "Введите данные сотрудника " << i + 1 << ":\n";
            std::cout << "ID: ";
            std::cin >> emp.num;
    
            bool duplicate = false;
            for (size_t j = 0; j < employees.size(); j++) {
                if (employees[j].num == emp.num) {
                    duplicate = true;
                    break;
                }
            }
            if (duplicate) {
                std::cerr << "Ошибка: сотрудник с таким ID уже существует. Повторите ввод.\n";
                continue;  
            }
    
            std::cout << "Имя (макс. 9 символов): ";
            std::cin >> emp.name;
            std::cout << "Отработанные часы: ";
            std::cin >> emp.hours;
            valid = true;
        }
        employees.push_back(emp);
    }

    if (!writeEmployeeRecords(filename, employees)) {
        std::cerr << "Ошибка при записи в файл." << std::endl;
        return 1;
    }

    std::cout << "Файл успешно создан." << std::endl;
    return 0;
}