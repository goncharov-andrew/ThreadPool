#include <iostream>
#include "thrpool.h"

void func(int i)
{
    std::cout << "number: " << i;
}

int main(int argc, char *argv[])
{

    ThrPool *pool = new ThrPool(5);

    for(int i = 0; i < 5; ++i)
    {
        pool->addTask(func, i);
    }

    delete pool;


}
