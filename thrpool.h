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
#include<memory>

class ThrPool
{
    bool flag;

    std::condition_variable mQueueCheck;

    std::mutex mLockQueueMutex;

    int mSize;

    std::vector<std::thread> mWorkThreads;

    std::queue<std::function<void()>> mTasks;

    void threadFunc();

public:
    ThrPool(int sizeOfTask);
    ~ThrPool();

    template<typename Callable, typename... Args>
    std::future<typename std::result_of<Callable(Args...)>::type> addTask(Callable&& func, Args&&... args)
    {
        typedef typename std::result_of<Callable(Args...)>::type retType;

        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        std::future<retType> ftTask = task->get_future();

        {
            std::unique_lock<std::mutex> locker(this->mLockQueueMutex);

            this->mTasks.push([task](){(*task)();});

            this->mQueueCheck.notify_one();
        }

        return ftTask;
    }
};

#endif // THRPOOL_H
