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
private:

    bool mFlag;

    std::condition_variable mQueueCheck;

    std::mutex mLockQueueMutex;

    size_t mSize;

    std::vector<std::thread> mWorkThreads;

    std::queue<std::function<void()>> mTasks;

    void threadFunc();

public:
    ThrPool(size_t sizeOfTask = 5);
    //ThrPool(const ThrPool& object) delete;
    //ThrPool& operator=(const ThrPool& object) delete;
    ~ThrPool();

    template<typename Callable, typename... Args>
    std::future<typename std::result_of<Callable(Args...)>::type> addTask(Callable&& func, Args&&... args)
    {
        using retType = typename std::result_of<Callable(Args...)>::type ;

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
