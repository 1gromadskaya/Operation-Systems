#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include "Employee.h"

// Утилита Reporter получает через командную строку:
// argv[1] – имя исходного бинарного файла,
// argv[2] – имя текстового файла отчета,
// argv[3] – оплата за час работы.
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: Reporter <binary_file_name> <report_file_name> <hourly_rate>" << std::endl;
        return 1;
    }

    const char* binFilename = argv[1];
    const char* reportFilename = argv[2];
    double hourlyRate = std::atof(argv[3]);

    std::vector<Employee> employees;
    if (!readEmployeeRecords(binFilename, employees)) {
        std::cerr << "Ошибка чтения бинарного файла." << std::endl;
        return 1;
    }

    // сортировка по id
    sortEmployees(employees);

    std::ofstream ofs(reportFilename);
    if (!ofs) {
        std::cerr << "Ошибка создания файла отчета." << std::endl;
        return 1;
    }

    ofs << "Отчет по файлу \"" << binFilename << "\"" << std::endl;
    ofs << "Номер сотрудника\tИмя сотрудника\tЧасы\tЗарплата" << std::endl;
    ofs << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < employees.size(); i++) {
        double salary = employees[i].hours * hourlyRate;
        ofs << employees[i].num << "\t" 
            << employees[i].name << "\t" 
            << employees[i].hours << "\t" 
            << salary << std::endl;
    }
    ofs.close();
    return 0;
}