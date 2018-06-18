#include <iostream>
#include "thrpool.h"
#include "fstream"
#include "string"
#include "QDebug"

#define SIZE 10

void func(int i)
{
    std::ofstream fout("./" + std::to_string(i) + ".txt");
    fout << std::to_string(i);
    fout.flush();
    fout.close();
}

int main(int argc, char *argv[])
{
    ThrPool *pool = new ThrPool(SIZE);

    for(int i = 0; i < SIZE + 10; ++i)
    {
        pool->addTask(func, i);
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(100));//sleep because ThrPool deleted before all tasks executed

    delete pool;

    return 0;
}
