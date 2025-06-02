#include "marker_logic.h"
#include <gtest/gtest.h>
#include <vector>
#include <chrono>
#include <thread> 
#include <numeric> 
#include <memory> 

class MarkerTest : public ::testing::Test {
protected:
    static constexpr int TEST_ARR_SIZE = 20;
    static constexpr int TEST_NUM_MARKERS = 3;

    std::unique_ptr<MarkerManager> manager;

    void SetUp() override {
        manager = std::make_unique<MarkerManager>(TEST_ARR_SIZE, TEST_NUM_MARKERS);
        manager->startMarkers();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void TearDown() override {
        manager.reset();
    }

     bool WaitForActiveCountDecrease(int target_count, std::chrono::milliseconds timeout) {
         auto start_time = std::chrono::steady_clock::now();
         while (std::chrono::steady_clock::now() - start_time < timeout) {
             if (manager->getActiveMarkerCount() == target_count) {
                 return true;
             }
             std::this_thread::sleep_for(std::chrono::milliseconds(20));
         }
         return manager->getActiveMarkerCount() == target_count; 
     }
};


TEST_F(MarkerTest, TerminationClearsMarkers) {
    ASSERT_NE(manager, nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    const int id_to_terminate = 1;
    ASSERT_TRUE(id_to_terminate > 0 && id_to_terminate <= TEST_NUM_MARKERS);

    int initial_active_markers = manager->getActiveMarkerCount();
    ASSERT_GT(initial_active_markers, 0); 

    ASSERT_TRUE(manager->requestTermination(id_to_terminate));

    
    ASSERT_TRUE(WaitForActiveCountDecrease(initial_active_markers - 1, std::chrono::seconds(2)))
        << "Active marker count did not decrease after termination request.";

    
    auto final_arr = manager->getArraySnapshot();
    for (int val : final_arr) {
        ASSERT_NE(val, id_to_terminate) << "Found marker of terminated thread "
                                        << id_to_terminate << " in array.";
    }
}


TEST_F(MarkerTest, AllThreadsTerminateCleanly) {
     ASSERT_NE(manager, nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(150)); 

    int initial_active = manager->getActiveMarkerCount();
     ASSERT_GT(initial_active, 0);

    manager->requestTerminationAll();
    manager->joinAll(); 

    ASSERT_EQ(manager->getActiveMarkerCount(), 0);

    
    auto final_arr = manager->getArraySnapshot();
    for (int val : final_arr) {
        ASSERT_EQ(val, 0) << "Array not fully cleared after all threads terminated.";
    }
}


TEST_F(MarkerTest, PauseResumeLogic) {
    ASSERT_NE(manager, nullptr);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    
    int expected_active = manager->getActiveMarkerCount();
     ASSERT_GT(expected_active, 0); 

    ASSERT_TRUE(manager->waitUntilAllPaused(std::chrono::seconds(5)))
        << "Not all active threads paused within timeout. Active count: "
        << manager->getActiveMarkerCount();

     
     ASSERT_EQ(manager->getActiveMarkerCount(), expected_active)
         << "Active marker count changed during waitUntilAllPaused.";


    
    manager->resumeAllPaused();
     std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

      
      
     ASSERT_EQ(manager->getActiveMarkerCount(), expected_active)
         << "Active marker count changed unexpectedly after resumeAllPaused.";
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}