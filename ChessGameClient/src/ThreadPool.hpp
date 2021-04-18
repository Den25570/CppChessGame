#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>

class ThreadPool {
public:
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
};
