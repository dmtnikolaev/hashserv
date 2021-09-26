#ifndef POOL_H
#define POOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>

class Pool {
    public:
        Pool(int size);
        ~Pool();
        void AddJob(std::tuple<void (*) (int) , int> new_job);
        void Shutdown();
    private:
        std::vector<std::thread> threads_;
        std::mutex queue_mutex_;
        std::mutex threadpool_mutex_;
        bool terminate_pool_ = false;
        bool stopped_ = false;
        std::queue<std::tuple<void (*) (int) , int>> queue_;
        std::condition_variable condition_;

        void InfiniteLoop();
};

#endif