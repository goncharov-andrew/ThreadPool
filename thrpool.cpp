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

    while(false != this->flag.test_and_set())
    {
        std::unique_lock<std::mutex> locker(this->mLockQueue);

        this->mQueueCheck.wait(locker);

        if(true != mTasks.empty())
        {
            exFunc = mTasks.front();
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


