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
#include<algorithm>
#include<iostream>
#include<map>
#include<list>


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

    //thread_priority_queue<, std::deque<TaskData>, LessThanByAge> mTasks;

    std::map<size_t, std::list<TaskData>> mTasks;

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
        std::future<T> mTask;

        Task(uint64_t id,  std::future<T>&& fTask) :
            mID(id),
            mTask(std::move(fTask))
        {}

    public:
        Task() = delete;
        Task(const Task&) = delete;
        Task& operator=(const Task& aTask) = delete;

        Task(Task&& aTask) :
            mID(aTask.mID),
            mTask(std::move(aTask.mTask))
        {     
        }

        Task& operator=(Task&& aTask)
        {
            mID = aTask.mID;
            mTask = std::move(aTask.mTask);

            return *this;
        }

        uint64_t getID() const
        {
            return mID;
        }

        const T getFutureTask()
        {
            return mTask.get();
        }
    };

    template<typename Callable, typename... Args>
    auto addTask(size_t priority, Callable&& func, Args&&... args) -> Task<typename std::result_of<Callable(Args...)>::type>
    {
        using retType = typename std::result_of<Callable(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        {
            std::unique_lock<std::mutex> locker(mLockQueueMutex);

            if(mTasks.end() == mTasks.find(priority))
            {
                mTasks[priority] = std::list<TaskData>();
            }

            mTasks[priority].push_back(TaskData(priority, mIDTaskCounter, [task](){(*task)();}));


            //mTasks.emplace(priority, mIDTaskCounter, [task](){(*task)();});

            ++mIDTaskCounter;

            mQueueCheck.notify_one();
        }

        Task<retType> retValue(mIDTaskCounter - 1, task->get_future());

        return retValue;
    }

    bool cancelTask(uint64_t id);
};
