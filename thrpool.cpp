#include "thrpool.h"

ThrPool::ThrPool(int sizeOfTask)
{
    this->mSize = sizeOfTask;

    for(int i = 0; i < this->mSize; ++i)
    {
        mWorkThreads.push_back(std::thread([this](){threadFunc();}));
    }
}

ThrPool::~ThrPool()
{
    flag.clear();

    for(int i = 0; i < this->mSize; ++i)
    {
        mWorkThreads[i].join();
    }
}

void ThrPool::threadFunc()
{
    std::function<void()> exFunc;
    std::mutex mutex;
    std::unique_lock<std::mutex> locker(mutex);

    while(false != this->flag.test_and_set())
    {
        this->mQueueCheck.wait(locker);

        mutex.lock();
        if(true != mTasks.empty())
        {
            exFunc = mTasks.front();
            mTasks.pop();

            mutex.unlock();

            exFunc();
        }
        else
        {
            mutex.unlock();
        }
    }
}

template<typename Callable, typename... Args>
auto ThrPool::addTask(Callable&& func, Args&&... args) -> std::future<decltype(func(args...))>
{
    std::packaged_task<Callable(Args...)> task(std::forward(func));

    std::future<decltype(func(args...))> ftTask = task.get_future();

    std::function<void()> templateFunc = std::bind(std::forward(task), std::forward(args...));


    std::mutex mutex;

    mutex.lock();

    this->mTasks.push(templateFunc);

    mutex.unlock();

    this->mQueueCheck.notify_one();

    return ftTask;
}
