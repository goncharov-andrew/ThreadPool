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
#include <algorithm>
#include <iostream>


class ThrPool
{
private:

    bool mFlag;

    uint64_t mIDTaskCounter;

    std::condition_variable mQueueCheck;

    std::mutex mLockQueueMutex;

    size_t mSize;

    std::vector<std::thread> mWorkThreads;

    class TaskData
    {
    private:
        int mPriority;
        uint64_t mID;
        std::function<void()> mExFunc;
    public:
        TaskData(int prio, uint64_t id, std::function<void()> &&func):
            mPriority(prio),
            mID(id),
            mExFunc(std::move(func))
        {}

        uint64_t getID() const
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
      bool operator()(TaskData& lhs, TaskData& rhs) const
      {
        return lhs.getPriority() < rhs.getPriority();
      }
    };

    template<typename DataType, typename Container, typename Compare>
    class thread_priority_queue : public std::priority_queue<DataType, Container, Compare>
    {
    public:
        bool remove(uint64_t& id)
        {
            auto it = std::find_if(this->c.begin(), this->c.end(), [id](DataType & element) -> bool { return id == element.getID();});
            if (it != this->c.end())
            {
                this->c.erase(it);
                std::make_heap(this->c.begin(), this->c.end(), this->comp);
                return true;
            }
            else
            {
                return false;
            }
        }
    };

    thread_priority_queue<TaskData, std::deque<TaskData>, LessThanByAge> mTasks;

    void threadFunc();

public:
    ThrPool(size_t sizeOfTask = 5);
    ~ThrPool();

    template<typename T>
    struct Task
    {
        friend class ThrPool;

    private:
        uint64_t mID;
        std::shared_ptr<std::packaged_task<T()>> mTask;

        Task(uint64_t id,  std::shared_ptr<std::packaged_task<T()>>& fTask) :
            mID(id),
            mTask(fTask)
        {
        }

    public:
        Task() = delete;
        //Task(const Task&) = delete;

        Task(const Task& aTask) :
            mID(aTask.mID),
            mTask(aTask.mTask)
        {
        }

        Task& operator=(const Task& aTask)
        {
            mID = aTask.mID;
            mTask = aTask.mTask;

            return *this;
        }

        //Task& operator=(const Task& aTask) = delete;

        uint64_t getID() const
        {
            return mID;
        }

        const T getFutureTask()
        {
            std::cerr << "ref_count: " << mTask.use_count() << "\n";
            return mTask->get_future().get();
        }
    };

    template<typename Callable, typename... Args>
    auto addTask(size_t priority, Callable&& func, Args&&... args) -> Task<typename std::result_of<Callable(Args...)>::type>
    {
        using retType = typename std::result_of<Callable(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        {
            std::unique_lock<std::mutex> locker(mLockQueueMutex);

            mTasks.emplace(priority, mIDTaskCounter, [task](){(*task)();});

            ++mIDTaskCounter;

            mQueueCheck.notify_one();
        }

        Task<retType> retValue(mIDTaskCounter - 1, task);

        return retValue;
    }

    bool cancelTask(uint64_t id);
};
