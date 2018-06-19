#include "thrpool.h"


ThrPool::ThrPool(size_t sizeOfTask)
{
    mSize = sizeOfTask;

    mFlag = true;

    for(size_t i = 0; i < mSize; ++i)
    {
        mWorkThreads.push_back(std::thread([this](){threadFunc();}));
    }
}

ThrPool::~ThrPool()
{
    {
        std::unique_lock<std::mutex> locker(mLockQueueMutex);

        mFlag = false;

        mQueueCheck.notify_all();
    }

    for(size_t i = 0; i < mWorkThreads.size(); ++i)
    {
        mWorkThreads[i].join();
    }
}

void ThrPool::threadFunc()
{  
    while(false != mFlag || true != mTasks.empty())
    {
        std::unique_lock<std::mutex> locker(mLockQueueMutex);

        if(false != mFlag)
        {
            mQueueCheck.wait(locker);
        }

        if(true != mTasks.empty())
        {
            std::function<void()> exFunc = mTasks.top().second;

            mTasks.pop();

            locker.unlock();

            exFunc();
        }
        else
        {
            locker.unlock();
        }
    }
}


