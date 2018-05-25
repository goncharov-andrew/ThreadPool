#include "thrpool.h"

ThrPool::ThrPool(int sizeOfTask)
{
    this->mSize = sizeOfTask;

    for(int i = 0; i < this->mSize; ++i)
    {
        mWorkThreads.push_back(std::thread(threadFunc, flag));
    }
}

ThrPool::~ThrPool()
{
    flag.clear();
}

void ThrPool::threadFunc(std::atomic_flag& thrFlag)
{
    while(false != thrFlag.test_and_set())
    {

    }
}

template<typename Callable, typename... Args>
std::future<decltype(func(args...))> ThrPool::addTask(Callable&& func, Args&&... args)
{
    std::packaged_task<Callable(Args...)> task(std::forward(func));

    std::future<decltype(func(args...))> ftTask = task.get_future();

    std::function<void()> templateFunc = std::bind(std::forward(task), std::forward(args...));

    this->mTasks.push(templateFunc);

    return ftTask;
}
