#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>

int NumThreads;
std::vector<std::thread> pool;
std::mutex queueMutex;
std::mutex poolMutex;
std::queue<std::function<void()>> jobQueue;
std::condition_variable condition;
bool stopped;
bool terminatePool;

void InitThreadPool();
void AddJob(std::function<void()> newJob);
void Shutdown();
void infiniteLoop();


void InitThreadPool()
{
    NumThreads = std::thread::hardware_concurrency();
    stopped = false;
    terminatePool = false;
    for (int ii = 0; ii < NumThreads; ii++)
    {
        pool.push_back(std::thread(infiniteLoop));
    }

}

void AddJob(std::function<void()> newJob)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobQueue.push(newJob);
    }
    condition.notify_one();
}

void Shutdown()
{
    {
        std::unique_lock<std::mutex> lock(poolMutex);
        terminatePool = true;
    }

    condition.notify_all();

    for (std::thread& thread : pool)
    {
        thread.join();
    }

    pool.clear();
    stopped = true;
}

void infiniteLoop()
{
    while (true)
    {
        std::function<void()> Job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [] {return jobQueue.empty() || terminatePool; });
            Job = jobQueue.front();
            jobQueue.pop();
        }
        Job();
    }
}
