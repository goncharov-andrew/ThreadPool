#include "thrpool.h"

ThrPool::ThrPool(int sizeOfTask)
{
    this->mSize = sizeOfTask;

    mWorkThreads.reserve(this->mSize);
}

template<typename Callable, typename... Args>
void ThrPool::addTask(Callable& func, Args&... args)
{

}
