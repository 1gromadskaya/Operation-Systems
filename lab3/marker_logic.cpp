
#include "marker_logic.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <numeric>
#include <functional>
#include <stdexcept> 

MarkerManager::MarkerManager(int arr_size, int num_markers)
    : m_arr(arr_size, 0),
      m_num_markers(num_markers),
      m_active_markers(num_markers),
      m_paused_count(0)
{
    if (arr_size <= 0 || num_markers <= 0) {
        throw std::invalid_argument("Array size and number of markers must be positive.");
    }
    m_thread_controls.resize(num_markers);
    for (int i = 0; i < num_markers; ++i) {
        m_thread_controls[i] = std::make_unique<ThreadControl>();
        m_thread_controls[i]->thread_id = i + 1;
    }
    m_threads.reserve(num_markers);
}

MarkerManager::~MarkerManager() {
    requestTerminationAll();
    joinAll();
}

void MarkerManager::startMarkers() {
    if (!m_threads.empty()) {
         return;
    }
    try {
        for (int i = 0; i < m_num_markers; ++i) {
            m_threads.emplace_back(&MarkerManager::markerThreadFunc, this, i + 1);
        }
    } catch (...) {
        requestTerminationAll();
        joinAll();
        throw;
    }
}

void MarkerManager::markerThreadFunc(int id) {
    if (id - 1 < 0 || id - 1 >= m_thread_controls.size() || !m_thread_controls[id - 1]) {
        return;
    }
    ThreadControl& control = *m_thread_controls[id - 1];
    std::mt19937 rng(std::random_device{}() + id);
    std::uniform_int_distribution<int> dist(0, m_arr.size() - 1);

    bool should_decrement_paused_count = false;

    while (!control.terminated.load()) {
        int index = dist(rng);

        if (index < 0 || index >= m_arr.size()) continue;

        bool marked = false;
        {
            std::lock_guard<std::mutex> arr_lock(m_arr_mutex);
            if (m_arr[index] == 0) {
                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
                 m_arr[index] = id;
                 marked = true;
            }
        }

        if (marked) {
            {
                std::lock_guard<std::mutex> indices_lock(control.indices_mutex);
                control.marked_indices.push_back(index);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        control.paused.store(true);
        should_decrement_paused_count = true;
        {
             std::lock_guard<std::mutex> pause_lock(m_pause_state_mutex);
             m_paused_count++;
             m_pause_state_cv.notify_one();
        }


        std::unique_lock<std::mutex> control_lock(control.mtx);
        control.cv.wait(control_lock, [&control]{
             return !control.paused.load() || control.terminated.load();
        });

        if (control.terminated.load()) {
             break;
        } else {
             
             should_decrement_paused_count = false; 
        }
    }


    if (should_decrement_paused_count && control.paused.load()) {
         
         std::lock_guard<std::mutex> pause_lock(m_pause_state_mutex);
         m_paused_count--;
         
    }


    {
        std::lock_guard<std::mutex> arr_lock(m_arr_mutex);
        std::lock_guard<std::mutex> indices_lock(control.indices_mutex);
        for (int idx : control.marked_indices) {
            if (idx >= 0 && idx < m_arr.size() && m_arr[idx] == id) {
                m_arr[idx] = 0;
            }
        }
        control.marked_indices.clear();
    }

    m_active_markers--;
    
    m_pause_state_cv.notify_one();
}


bool MarkerManager::waitUntilAllPaused(std::chrono::milliseconds timeout) {
     std::unique_lock<std::mutex> lock(m_pause_state_mutex);
     return m_pause_state_cv.wait_for(lock, timeout, [this]{
         int current_active = m_active_markers.load();
         return m_paused_count == current_active || current_active == 0;
     });
}

void MarkerManager::resumeAllPaused() {
     std::lock_guard<std::mutex> pause_lock(m_pause_state_mutex);
     m_paused_count = 0; 

     for (auto& control_ptr : m_thread_controls) {
         if (control_ptr) {
             ThreadControl& control = *control_ptr;
              
             if (control.terminated.load()) continue;

             std::lock_guard<std::mutex> control_lock(control.mtx);
              
             if (control.paused.load()) {
                 control.paused.store(false);
                 control.cv.notify_one();
             }
         }
     }
}

bool MarkerManager::requestTermination(int marker_id) {
    if (marker_id <= 0 || marker_id > m_num_markers || !m_thread_controls[marker_id - 1]) {
        return false;
    }

    ThreadControl& control = *m_thread_controls[marker_id - 1];

    
    if (control.terminated.load()) {
        return false;
    }

    bool was_paused = false;
    {
        std::lock_guard<std::mutex> control_lock(control.mtx);
        
        if (control.terminated.load()) {
            return false;
        }
        control.terminated.store(true);
        was_paused = control.paused.exchange(false);
        control.cv.notify_one();
    }


    if (was_paused) {
        std::lock_guard<std::mutex> pause_lock(m_pause_state_mutex);
        m_paused_count--;
        m_pause_state_cv.notify_one();
    }
    return true;
}

void MarkerManager::requestTerminationAll() {
     for (int i = 0; i < m_num_markers; ++i) {
         if (m_thread_controls[i]) {
            
            ThreadControl& control = *m_thread_controls[i];
            if (control.terminated.load()) continue; 

             bool was_paused = false;
             {
                 std::lock_guard<std::mutex> control_lock(control.mtx);
                 if (control.terminated.load()) continue; 
                 control.terminated.store(true);
                 was_paused = control.paused.exchange(false);
                 control.cv.notify_one();
             }

             if (was_paused) {
                 std::lock_guard<std::mutex> pause_lock(m_pause_state_mutex);
                 m_paused_count--;
                 m_pause_state_cv.notify_one();
             }
         }
     }
}


void MarkerManager::joinAll() {
    for (auto& t : m_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
     
     m_threads.clear();
}

std::vector<int> MarkerManager::getArraySnapshot() const {
    std::lock_guard<std::mutex> lock(m_arr_mutex);
    return m_arr;
}

int MarkerManager::getActiveMarkerCount() const {
    return m_active_markers.load();
}

int MarkerManager::getTotalMarkerCount() const {
    return m_num_markers;
}