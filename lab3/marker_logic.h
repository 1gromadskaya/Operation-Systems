
#ifndef MARKER_LOGIC_H
#define MARKER_LOGIC_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <atomic> 




struct ThreadControl {
    std::atomic<bool> paused{false}; 
    std::atomic<bool> terminated{false}; 
    std::condition_variable cv;
    std::mutex mtx; 
    int thread_id = 0;
     
    std::vector<int> marked_indices;
    std::mutex indices_mutex; 
};

class MarkerManager {
public:
    
    MarkerManager(int arr_size, int num_markers);

    
    ~MarkerManager();

    
    void startMarkers();

    
    
    bool waitUntilAllPaused(std::chrono::milliseconds timeout = std::chrono::seconds(10));

    
    void resumeAllPaused();

    
    
    bool requestTermination(int marker_id);

    
    void requestTerminationAll();

    
    void joinAll();

    
    std::vector<int> getArraySnapshot() const;

    
    int getActiveMarkerCount() const;

    
    int getTotalMarkerCount() const;


private:
    
    void markerThreadFunc(int id);

    
    std::vector<int> m_arr;
    mutable std::mutex m_arr_mutex; 

    std::vector<std::unique_ptr<ThreadControl>> m_thread_controls;
    std::vector<std::thread> m_threads;

    std::atomic<int> m_active_markers; 
    const int m_num_markers; 

    
    std::mutex m_pause_state_mutex;
    std::condition_variable m_pause_state_cv;
    int m_paused_count = 0;

};

#endif 