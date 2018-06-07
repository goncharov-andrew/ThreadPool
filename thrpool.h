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
    std::future<typename std::result_of<Callable(Args...)>::type> addTask(Callable&& func, Args&&... args)
    {
        typedef typename std::result_of<Callable(Args...)>::type retType;

        auto task = std::make_shared<std::packaged_task<retType>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        auto ftTask = task->get_future();

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
