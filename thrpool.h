#pragma once

#include<thread>
#include<vector>
#include<queue>
#include<future>
#include<functional>
#include<atomic>
#include<mutex>
#include<condition_variable>
#include<memory>

template<typename T>
struct TaskData
{
private:
    long long mID;
    T resTask;
public:
    TaskData(long long id, T &&fTask):
        mID(id)
    {
        resTask = std::move(fTask);
    }
    long long getID() const
    {
        return mID;
    }
    /*T getFutureTask()
    {
        return resTask;
    }*/
};

class ThrPool
{
private:

    bool mFlag;

    long long mIDTask;

    std::condition_variable mQueueCheck;

    std::mutex mLockQueueMutex;

    size_t mSize;

    std::vector<std::thread> mWorkThreads;

    class Task
    {
    private:
        int mPriority;
        long long mID;
        std::function<void()> mExFunc;
    public:
        Task(int prio, long long id, std::function<void()> func):
            mPriority(prio),
            mID(id),
            mExFunc(func)
        {}

        long long getID() const
        {
            return mID;
        }
        int getPriority() const
        {
            return mPriority;
        }
        std::function<void()> getExFunc() const
        {
            return mExFunc;
        }
    };

    struct LessThanByAge
    {
      bool operator()(Task& lhs, Task& rhs) const
      {
        return lhs.getPriority() < rhs.getPriority();
      }
    };

    std::priority_queue<size_t, std::deque<Task>, LessThanByAge> mTasks;

    void threadFunc();

public:
    ThrPool(size_t sizeOfTask = 5);
    ~ThrPool();

    template<typename Callable, typename... Args>
    auto addTask(size_t priority, Callable&& func, Args&&... args) -> TaskData<std::future<typename std::result_of<Callable(Args...)>::type>>
    {
        using retType = typename std::result_of<Callable(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        {
            std::unique_lock<std::mutex> locker(mLockQueueMutex);

            mTasks.push(Task(priority, mIDTask, [task](){(*task)();}));

            ++mIDTask;

            mQueueCheck.notify_one();
        }

        return TaskData<std::future<retType>>(mIDTask - 1, task->get_future());
    }

    bool cancelTask(long long id);
};
