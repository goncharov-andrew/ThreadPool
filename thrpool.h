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



    struct LessThanByAge
    {
      bool operator()(const std::pair<size_t, std::function<void()>>& lhs, const std::pair<size_t, std::function<void()>>& rhs) const
      {
        return lhs.first < rhs.first;
      }
    };

    std::priority_queue<size_t, std::deque<std::pair<size_t, std::function<void()>>>, LessThanByAge> mTasks;

    void threadFunc();

public:
    ThrPool(size_t sizeOfTask = 5);
    ~ThrPool();

    template<typename Callable, typename... Args>
    std::future<typename std::result_of<Callable(Args...)>::type> addTask(size_t priority, Callable&& func, Args&&... args)
    {
        using retType = typename std::result_of<Callable(Args...)>::type ;

        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        std::future<retType> ftTask = task->get_future();

        {
            std::unique_lock<std::mutex> locker(mLockQueueMutex);

            mTasks.push(std::make_pair(priority, [task](){(*task)();}));

            mQueueCheck.notify_one();
        }

        return ftTask;
    }
};

#endif // THRPOOL_H
