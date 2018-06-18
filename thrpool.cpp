#include "thrpool.h"


ThrPool::ThrPool(int sizeOfTask)
{
    this->mSize = sizeOfTask;

    this->flag = true;

    for(int i = 0; i < this->mSize; ++i)
    {
        mWorkThreads.push_back(std::thread([this](){threadFunc();}));
    }
}

ThrPool::~ThrPool()
{
    {
        std::unique_lock<std::mutex> locker(this->mLockQueueMutex);

        this->flag = false;

        this->mQueueCheck.notify_all();
    }

    for(size_t i = 0; i < mWorkThreads.size(); ++i)
    {
        mWorkThreads[i].join();
    }
}

void ThrPool::threadFunc()
{  
    while(false != this->flag)
    {
        while(true != mTasks.empty())
        {
            std::unique_lock<std::mutex> locker(this->mLockQueueMutex);

            if(false != this->flag)
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
}


