#include "ThreadPool.hpp"


void ThreadPool::InitThreadPool()
{
    NumThreads = std::thread::hardware_concurrency();
    stopped = false;
    terminatePool = false;
    for (int ii = 0; ii < NumThreads; ii++)
    {
        pool.push_back(std::thread(std::bind(&ThreadPool::infiniteLoop, this)));
    }

}

void ThreadPool::AddJob(std::function<void()> newJob)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobQueue.push(newJob);
    }
    condition.notify_one();
}

void ThreadPool::Shutdown()
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

void ThreadPool::infiniteLoop()
{
    while (true)
    {
        std::function<void()> Job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this] {return !jobQueue.empty() || terminatePool; });
            if (terminatePool) {
                break;
            }
            Job = jobQueue.front();
            jobQueue.pop();
        }
        Job();
    }
}
