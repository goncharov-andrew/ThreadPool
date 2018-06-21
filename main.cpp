#include <iostream>
#include "thrpool.h"
#include "fstream"
#include "string"
#include "QDebug"
#include "vector"

#define SIZE 10

int func(int i)
{
    std::ofstream fout("./" + std::to_string(i) + ".txt");
    fout << std::to_string(i);
    fout.flush();
    fout.close();

    return 1;
}

int main(int argc, char *argv[])
{
    ThrPool pool(SIZE);

    std::vector<ThrPool::Task<int> > a;

    std::vector<int> answers;

    for(size_t i = 0; i < SIZE + 10; ++i)
    {
        a.push_back(pool.addTask(i, func, i));
    }

   /* for(auto it = a.begin(); it != a.end(); ++it)
    {
        answers.push_back(it->resTask.get());
    }*/

    return 0;
}
