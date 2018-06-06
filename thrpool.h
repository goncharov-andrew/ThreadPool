#ifndef THRPOOL_H
#define THRPOOL_H

#include<thread>
#include<vector>
#include<queue>
#include<future>
#include<functional>
#include<atomic>
#include<mutex>
#include<condition_variable>

class ThrPool
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

    std::condition_variable mQueueCheck;

    std::mutex mLockQueue;

    int mSize;

    std::vector<std::thread> mWorkThreads;

    std::queue<std::function<void()>> mTasks;

    void threadFunc();

public:
    ThrPool(int sizeOfTask);
    ~ThrPool();

    template<typename Callable, typename... Args>
    auto addTask(Callable&& func, Args&&... args) -> std::future<decltype(func(args...))>
    {
        std::packaged_task<decltype(func(args...))(args...)> task(std::forward<decltype(func(args...))>(decltype(func)));

        std::future<decltype(func(args...))> ftTask = task.get_future();

        std::function<void()> templateFunc = std::bind(std::forward(task), std::forward<Args>(args)...);

        {
            std::unique_lock<std::mutex> locker(this->mLockQueue);

            this->mTasks.push(templateFunc);
        }

        this->mQueueCheck.notify_one();


        return ftTask;
    }
};

#endif // THRPOOL_H
