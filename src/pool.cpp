#include "pool.h"

#include <mutex>
#include <thread>
#include <functional>

Pool::Pool(int size) {
    for (auto i = 0; i < size; i++) {
        threads_.push_back(std::thread(&Pool::InfiniteLoop, this));
    }
}

Pool::~Pool() {
    if (!stopped_) {
        Shutdown();
    }
}

void Pool::InfiniteLoop() {
    while (true) {
        std::tuple<void (*) (int) , int> job;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            condition_.wait(lock, [this](){
                return !queue_.empty() || terminate_pool_;
            });
            job = queue_.front();
            queue_.pop();
        }

        std::get<0>(job)(std::get<1>(job));
    }
}

void Pool::AddJob(std::tuple<void (*) (int) , int> new_job) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_.push(new_job);
    }

    condition_.notify_one();
}

void Pool::Shutdown() {
    {
        std::unique_lock<std::mutex> lock(threadpool_mutex_);
        terminate_pool_ = true; // use this flag in condition.wait
    }

    condition_.notify_all(); // wake up all threads.

    // Join all threads.
    for (std::thread &th : threads_) {
        th.join();
    }

    threads_.clear();  
    stopped_ = true; // use this flag in destructor, if not set, call shutdown()
}