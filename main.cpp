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
    ThrPool pool(SIZE);

    for(size_t i = 0; i < SIZE + 10; ++i)
    {
        pool.addTask(func, i);
    }

    return 0;
}
