#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <limits>

const int MAX_MESSAGE_LEN = 20;
const int MESSAGE_SLOT_SIZE = MAX_MESSAGE_LEN + 1;

struct FileHeader {
    int max_messages;
    int read_pos;
    int write_pos;
};

int main(int argc, char *argv[]) {

    if (argc != 6) { /* ... Usage ... */ return 1; }

    std::string shared_file_name = argv[1];
    const char* sem_empty_name = argv[2];
    const char* sem_filled_name = argv[3];
    const char* sem_mutex_name = argv[4];
    const char* sem_ready_name = argv[5];
    pid_t my_pid = getpid();

    std::cout << "[Sender PID: " << my_pid << "] Started." << std::endl;
    std::cout << "[Sender PID: " << my_pid << "] File Path: " << shared_file_name << std::endl;
    std::cout << "[Sender PID: " << my_pid << "] Semaphores: " << sem_empty_name << ", "
              << sem_filled_name << ", " << sem_mutex_name << ", " << sem_ready_name << std::endl;

    sem_t *sem_empty = sem_open(sem_empty_name, 0);
    sem_t *sem_filled = sem_open(sem_filled_name, 0);
    sem_t *sem_mutex = sem_open(sem_mutex_name, 0);
    sem_t *sem_ready = sem_open(sem_ready_name, 0);

    if (sem_empty == SEM_FAILED || sem_filled == SEM_FAILED || sem_mutex == SEM_FAILED || sem_ready == SEM_FAILED) {
        return 1;
    }
    std::cout << "[Sender PID: " << my_pid << "] Semaphores opened successfully." << std::endl;

    char command;
    std::string message;
    FileHeader header;

    do {
        std::cout << "[Sender PID: " << my_pid << "] Enter command: (S)end, (Q)uit: ";
        std::cin >> command;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        command = toupper(command);

        if (command == 'S') {
            std::cout << "Enter message (max " << MAX_MESSAGE_LEN << " chars): ";
            std::getline(std::cin, message);
            if (message.length() > MAX_MESSAGE_LEN) { /* ... truncate ... */ }

            std::cout << "[Sender PID: " << my_pid << "] Waiting for an empty slot..." << std::endl;
            if (sem_wait(sem_empty) == -1) {
                 if (errno == EINTR) continue;
                 std::cerr << "Error sem_wait(sem_empty): " << strerror(errno) << std::endl; break;
            }
             std::cout << "[Sender PID: " << my_pid << "] Empty slot acquired." << std::endl;

             std::cout << "[Sender PID: " << my_pid << "] Waiting for mutex..." << std::endl;
            if (sem_wait(sem_mutex) == -1) {
                 if (errno == EINTR) { sem_post(sem_empty); continue; }
                 std::cerr << "Error sem_wait(sem_mutex): " << strerror(errno) << std::endl; sem_post(sem_empty); break;
            }
            std::cout << "[Sender PID: " << my_pid << "] Mutex acquired." << std::endl;

            bool file_op_success = true;
            std::fstream shared_file_op;
            shared_file_op.open(shared_file_name, std::ios::binary | std::ios::in | std::ios::out);
            if(!shared_file_op.is_open()) {
                 std::cerr << "Error opening file: " << strerror(errno) << std::endl;
                 file_op_success = false;
            } else {
                shared_file_op.seekg(0);
                shared_file_op.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));
                if (!shared_file_op.good()) {
                    std::cerr << "Error reading file header." << std::endl;
                    file_op_success = false;
                } else {
                    long write_offset = sizeof(FileHeader) + (long)header.write_pos * MESSAGE_SLOT_SIZE;
                    shared_file_op.seekp(write_offset);

                    char message_slot[MESSAGE_SLOT_SIZE] = {0};
                    strncpy(message_slot, message.c_str(), MAX_MESSAGE_LEN);
                    message_slot[MAX_MESSAGE_LEN] = '\0';

                    shared_file_op.write(message_slot, MESSAGE_SLOT_SIZE);
                    shared_file_op.flush();

                    if (!shared_file_op.good()) {
                         std::cerr << "Error writing message to file." << std::endl;
                         file_op_success = false;
                    } else {
                        int old_write_pos = header.write_pos;
                        header.write_pos = (header.write_pos + 1) % header.max_messages;
                        shared_file_op.seekp(0);
                        shared_file_op.write(reinterpret_cast<const char*>(&header), sizeof(FileHeader));
                        shared_file_op.flush();

                        if (!shared_file_op.good()) {
                             std::cerr << "Error writing updated header." << std::endl;
                             file_op_success = false;
                        } else {
                             std::cout << "[Sender PID: " << my_pid << "] Message written to slot " << old_write_pos
                                       << ", new write_pos=" << header.write_pos << std::endl;
                        }
                    }
                }
                 shared_file_op.close();
            }

            if (sem_post(sem_mutex) == -1) {
                std::cerr << "Error sem_post(sem_mutex): " << strerror(errno) << std::endl;
                file_op_success = false;
            } else {
                 std::cout << "[Sender PID: " << my_pid << "] Mutex released." << std::endl;
            }

            if (file_op_success) {
                if (sem_post(sem_filled) == -1) {
                    std::cerr << "Error sem_post(sem_filled): " << strerror(errno) << std::endl;
                } else {
                     std::cout << "[Sender PID: " << my_pid << "] Filled slot signaled." << std::endl;
                }
            } else {
                 sem_post(sem_empty);
                 std::cout << "[Sender PID: " << my_pid << "] Operation failed, empty slot returned." << std::endl;
            }

        } else if (command == 'Q') {
            std::cout << "[Sender PID: " << my_pid << "] Quit command received." << std::endl;
        } else {
            std::cout << "[Sender PID: " << my_pid << "] Unknown command." << std::endl;
        }

    } while (command != 'Q');

    std::cout << "[Sender PID: " << my_pid << "] Closing resources..." << std::endl;
    sem_close(sem_empty);
    sem_close(sem_filled);
    sem_close(sem_mutex);
    sem_close(sem_ready);
    std::cout << "[Sender PID: " << my_pid << "] Exiting." << std::endl;

    return 0;
}