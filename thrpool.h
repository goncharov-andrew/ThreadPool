#ifndef THRPOOL_H
#define THRPOOL_H

#include<thread>
#include<vector>
#include<queue>

template<typename Callable, typename... Args>
using TaskQueue = std::pair<Callable& , Args...>;

class ThrPool
{
    int mSize;
    std::vector<std::thread> mWorkThreads;

    std::queue<TaskQueue> mTasks;

public:
    ThrPool(int sizeOfTask);

    template<typename Callable, typename... Args>
    void addTask(Callable& func, Args&... args);
};

#endif // THRPOOL_H
