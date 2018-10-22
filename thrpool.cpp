#include "thrpool.h"


ThrPool::ThrPool(size_t NumberOfThreads):
    mFlag(true),
    mIDTaskCounter(0),
    mQueueCheck(),
    mLockQueueMutex(),
    mNumberOfThreads(NumberOfThreads)
{
    for(size_t i = 0; i < mNumberOfThreads; ++i)
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
    while(true)
    {
        std::unique_lock<std::mutex> locker(mLockQueueMutex);

        if(true != mTasks.empty())
        {
            std::function<void()> exFunc;

            exFunc = std::move(mTasks.top().getExFunc());
            mTasks.pop();

            locker.unlock();

            exFunc();
        }
        else
        {
            locker.unlock();

            if(false == mFlag)
            {
                break;
            }
            else
            {
                std::unique_lock<std::mutex> locker(mLockQueueMutex);

                if(true == mTasks.empty())
                {
                    mQueueCheck.wait(locker);
                }
            }
        }
    }
}
