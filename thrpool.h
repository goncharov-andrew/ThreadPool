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

#include <gtest/gtest_prod.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>

class ThrPool
{
private:

    FRIEND_TEST(FirstTestCase, FirstTestSet);

    bool mFlag;

    uint64_t mIDTaskCounter;

    std::condition_variable mQueueCheck;

    std::mutex mLockQueueMutex;

    size_t mNumberOfThreads;

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

         struct ByID {}; struct ByPriority {};
    };

    typedef boost::multi_index::multi_index_container<
    TaskData,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<boost::multi_index::tag<TaskData::ByID>, boost::multi_index::const_mem_fun<TaskData, uint64_t, &TaskData::getID>>,
        boost::multi_index::ordered_non_unique<boost::multi_index::tag<TaskData::ByPriority>, boost::multi_index::const_mem_fun<TaskData, int, &TaskData::getPriority>>
        >
    > TaskContainer;

    TaskContainer mTasks;

    void threadFunc();

public:
    ThrPool(size_t NumberOfThreads = 5);
    ~ThrPool();

    template<typename T>
    struct Task
    {
        friend class ThrPool;

    private:
        uint64_t mID;
        std::future<T> mTask;
        bool isTaskValid;

        Task(uint64_t id,  std::future<T>&& fTask) :
            mID(id),
            mTask(std::move(fTask)),
            isTaskValid(true)
        {}

    public:
        Task() = delete;
        Task(const Task&) = delete;
        Task& operator=(const Task& aTask) = delete;

        Task(Task&& aTask) :
            mID(aTask.mID),
            mTask(std::move(aTask.mTask)),
            isTaskValid(aTask.isTaskValid)
        {     
        }

        Task& operator=(Task&& aTask)
        {
            mID = aTask.mID;
            mTask = std::move(aTask.mTask);
            isTaskValid = aTask.isTaskValid;

            return *this;
        }

        uint64_t getID() const
        {
            return mID;
        }
    };

    template<typename Callable, typename... Args>
    auto addTask(size_t priority, Callable&& func, Args&&... args) -> Task<typename std::result_of<Callable(Args...)>::type>
    {
        using retType = typename std::result_of<Callable(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

        {
            std::unique_lock<std::mutex> locker(mLockQueueMutex);

            mTasks.insert(TaskData(priority, mIDTaskCounter, [task](){(*task)();}));

            ++mIDTaskCounter;

            mQueueCheck.notify_one();
        }

        Task<retType> retValue(mIDTaskCounter - 1, task->get_future());

        return retValue;
    }

    template<typename T>
    bool cancelTask(Task<T>& task)
    {
        bool retValue = false;

        {
            std::unique_lock<std::mutex> locker(mLockQueueMutex);

            TaskContainer::index<TaskData::ByID>::type& sortByID = mTasks.get<TaskData::ByID>();

            TaskContainer::index<TaskData::ByID>::type::iterator it = sortByID.find(task.getID());

            if(it != sortByID.end())
            {
                sortByID.erase(task.getID());

                task.isTaskValid = false;
                retValue = true;
            }
        }

        return retValue;
    }

    template<typename T>
    std::pair<bool, T> getResultOfTask(Task<T>&& task)
    {
        std::pair<bool, T> retValue;

        if(true == task.isTaskValid)
        {
            retValue.first = true;
            retValue.second = task.mTask.get();
            task.isTaskValid = false;
        }
        else
        {
            retValue.first = false;
        }

        return retValue;
    }

    std::pair<bool, int> getResultOfTask(Task<void>&& task)
    {
        std::pair<bool, int> retValue;

        if(true == task.isTaskValid)
        {
            retValue.first = true;
            retValue.second = 1;
            task.isTaskValid = false;
        }
        else
        {
            retValue.first = false;
            retValue.second = -1;
        }

        return retValue;
    }
};
