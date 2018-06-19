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

struct LessThanByAge
{
  bool operator()(const std::pair<int, std::function<void()>>& lhs, const std::pair<int, std::function<void()>>& rhs) const
  {
    return lhs.first < rhs.first;
  }
};

class ThrPool
{
private:

    bool mFlag;

    std::condition_variable mQueueCheck;

    std::mutex mLockQueueMutex;

    size_t mSize;

    std::vector<std::thread> mWorkThreads;

    std::priority_queue<int, std::vector<std::pair<int, std::function<void()>>>, LessThanByAge> mTasks;

    void threadFunc();

public:
    ThrPool(size_t sizeOfTask = 5);
    ThrPool(const ThrPool& object) = delete;
    ThrPool& operator=(const ThrPool& object) = delete;
    ~ThrPool();

    template<typename Callable, typename... Args>
    std::future<typename std::result_of<Callable(Args...)>::type> addTask(int priority, Callable&& func, Args&&... args)
    {
        using retType = typename std::result_of<Callable(Args...)>::type ;

        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        std::future<retType> ftTask = task->get_future();

        {
            std::unique_lock<std::mutex> locker(this->mLockQueueMutex);

            this->mTasks.push(std::make_pair(priority, [task](){(*task)();}));

            this->mQueueCheck.notify_one();
        }

        return ftTask;
    }
};

#endif // THRPOOL_H
