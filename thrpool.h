#ifndef THRPOOL_H
#define THRPOOL_H

#include<thread>
#include<vector>
#include<queue>
#include<future>
#include<functional>
#include<atomic>

class ThrPool
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

    int mSize;

    std::vector<std::thread> mWorkThreads;

    std::queue<std::function<void()>> mTasks;

    static void threadFunc(std::atomic_flag &thrFlag);

public:
    ThrPool(int sizeOfTask);
    ~ThrPool();

    template<typename Callable, typename... Args>
    auto addTask(Callable&& func, Args&&... args) -> std::future<decltype(func(args...))>;
};

#endif // THRPOOL_H
