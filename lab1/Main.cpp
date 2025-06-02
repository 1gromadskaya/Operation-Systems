#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif
#include "Employee.h"

void printBinaryFile(const char* filename) {
    std::vector<Employee> employees;
    if (readEmployeeRecords(filename, employees)) {
        std::cout << "Содержимое бинарного файла:" << std::endl;
        for (size_t i = 0; i < employees.size(); i++) {
            std::cout << "ID: " << employees[i].num 
                      << ", Имя: " << employees[i].name 
                      << ", Часы: " << employees[i].hours << std::endl;
        }
    } else {
        std::cerr << "Ошибка чтения бинарного файла." << std::endl;
    }
}

void printReport(const char* filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "Ошибка открытия файла отчета." << std::endl;
        return;
    }
    std::string line;
    std::cout << "Содержимое файла отчета:" << std::endl;
    while (std::getline(ifs, line)) {
        std::cout << line << std::endl;
    }
}

int runProcess(const std::string &command) {
#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char* cmd = new char[command.size() + 1];
    std::strcpy(cmd, command.c_str());

    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Не удалось запустить процесс: " << command << std::endl;
        delete[] cmd;
        return 1;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    delete[] cmd;
    return 0;
#else
    int ret = system(command.c_str());
    if (ret != 0) {
        std::cerr << "Ошибка при выполнении команды: " << command << std::endl;
    }
    return ret;
#endif
}

int main() {
    char binFilename[256];
    int numRecords;

    std::cout << "Введите имя бинарного файла: ";
    std::cin >> binFilename;
    std::cout << "Введите количество записей: ";
    std::cin >> numRecords;

    std::ostringstream creatorCmd;
#ifdef _WIN32
    creatorCmd << "Creator.exe " << binFilename << " " << numRecords;
#else
    creatorCmd << "./Creator " << binFilename << " " << numRecords;
#endif

    if (runProcess(creatorCmd.str()) != 0) {
        std::cerr << "Процесс Creator завершился с ошибкой." << std::endl;
        return 1;
    }

    printBinaryFile(binFilename);

    char reportFilename[256];
    double hourlyRate;
    std::cout << "Введите имя файла отчета: ";
    std::cin >> reportFilename;
    std::cout << "Введите оплату за час работы: ";
    std::cin >> hourlyRate;

    std::ostringstream reporterCmd;
#ifdef _WIN32
    reporterCmd << "Reporter.exe " << binFilename << " " << reportFilename << " " << hourlyRate;
#else
    reporterCmd << "./Reporter " << binFilename << " " << reportFilename << " " << hourlyRate;
#endif

    if (runProcess(reporterCmd.str()) != 0) {
        std::cerr << "Процесс Reporter завершился с ошибкой." << std::endl;
        return 1;
    }

    printReport(reportFilename);

    return 0;
}