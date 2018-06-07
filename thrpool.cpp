#include "thrpool.h"

ThrPool::ThrPool(int sizeOfTask)
{
    this->mSize = sizeOfTask;

    mFlafnotified = false;

    for(int i = 0; i < this->mSize; ++i)
    {
        mWorkThreads.push_back(std::thread([this](){threadFunc();}));
    }
}

ThrPool::~ThrPool()
{
    flag.clear();

    this->mQueueCheck.notify_all();

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

        while(!this->mFlafnotified)
        {
            this->mQueueCheck.wait(locker);
        }

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

        this->mFlafnotified = false;
    }

    int a = 1;
}


