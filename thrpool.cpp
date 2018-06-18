#include "thrpool.h"


ThrPool::ThrPool(size_t sizeOfTask)
{
    this->mSize = sizeOfTask;

    this->mFlag = true;

    for(size_t i = 0; i < this->mSize; ++i)
    {
        mWorkThreads.push_back(std::thread([this](){threadFunc();}));
    }
}

ThrPool::~ThrPool()
{
    {
        std::unique_lock<std::mutex> locker(this->mLockQueueMutex);

        this->mFlag = false;

        this->mQueueCheck.notify_all();
    }

    for(size_t i = 0; i < mWorkThreads.size(); ++i)
    {
        mWorkThreads[i].join();
    }
}

void ThrPool::threadFunc()
{  
    while(false != this->mFlag || true != mTasks.empty())
    {
        std::unique_lock<std::mutex> locker(this->mLockQueueMutex);

        if(false != this->mFlag)
        {
            this->mQueueCheck.wait(locker);
        }

        if(true != mTasks.empty())
        {
            std::function<void()> exFunc = mTasks.front();

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


