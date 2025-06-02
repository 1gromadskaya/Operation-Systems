#include "marker_logic.h"
#include <iostream>
#include <vector>
#include <limits>
#include <ios>
#include <string> 

void clear_cin() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void print_array(const std::vector<int>& arr) {
    std::cout << "Array state: ";
    for (int val : arr) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

int main() {
    int arr_size, num_markers;

    std::cout << "Enter array size: ";
    while (!(std::cin >> arr_size) || arr_size <= 0) {
        std::cout << "Invalid input. Please enter a positive integer for array size: ";
        std::cin.clear();
        clear_cin();
    }
    clear_cin();

    std::cout << "Enter number of marker threads: ";
    while (!(std::cin >> num_markers) || num_markers <= 0) {
        std::cout << "Invalid input. Please enter a positive integer for marker count: ";
        std::cin.clear();
        clear_cin();
    }
    clear_cin();


    try {
        MarkerManager manager(arr_size, num_markers);
        manager.startMarkers();

        while (manager.getActiveMarkerCount() > 0) {

            bool all_paused = manager.waitUntilAllPaused();

            int current_active = manager.getActiveMarkerCount();
            if (current_active == 0) break;

            if (!all_paused) {
                 std::cout << "Warning: Not all active threads paused within the timeout." << std::endl;
            }

            print_array(manager.getArraySnapshot());

            std::cout << "Enter thread ID to terminate (1-" << manager.getTotalMarkerCount()
                      << ", 0 to terminate all): ";
            int id_to_terminate;

            while (!(std::cin >> id_to_terminate) || id_to_terminate < 0 || id_to_terminate > manager.getTotalMarkerCount()) {
                 std::cout << "Invalid input. Please enter an integer between 0 and "
                           << manager.getTotalMarkerCount() << ": ";
                 std::cin.clear();
                 clear_cin();
            }
            clear_cin();


            if (id_to_terminate == 0) {
                std::cout << "Requesting termination of all threads..." << std::endl;
                manager.requestTerminationAll();
                break;
            } else {
                 std::cout << "Requesting termination of thread " << id_to_terminate << "..." << std::endl;
                 if (!manager.requestTermination(id_to_terminate)) {
                    std::cout << "Thread " << id_to_terminate
                              << " could not be terminated (might be invalid or already terminated)."
                              << std::endl;
                 }
                 // Resume others regardless of whether termination was successful for the specific ID
                 // If ID was invalid, resumeAllPaused will just resume all non-terminated.
                 manager.resumeAllPaused();
            }
        }

        std::cout << "Waiting for all threads to finish..." << std::endl;
        manager.joinAll();

        std::cout << "Final array state:" << std::endl;
        print_array(manager.getArraySnapshot());

        std::cout << "All marker threads terminated. Exiting..." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        return 2;
    }

    return 0;
}