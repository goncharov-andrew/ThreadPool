#include "thrpool.h"


ThrPool::ThrPool(size_t sizeOfTask):
    mFlag(true),
    mIDTaskCounter(0),
    mQueueCheck(),
    mLockQueueMutex(),
    mSize(sizeOfTask)
{
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
    while(false != mFlag)
    {
        std::unique_lock<std::mutex> locker(mLockQueueMutex);

        if(true == mTasks.empty())
        {
            mQueueCheck.wait(locker);
        }

        if(true != mTasks.empty())
        {
            std::function<void()> exFunc;

            for(size_t i = 0; i < mTasks.size(); ++i)
            {
                if(false == mTasks[i].empty())
                {
                    exFunc = std::move(mTasks[i].front().getExFunc());
                    mTasks[i].pop_front();

                    locker.unlock();

                    exFunc();

                    break;
                }
            }
        }
        else
        {
            locker.unlock();
        }
    }
}
