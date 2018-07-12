#include <iostream>
#include "thrpool.h"
#include "fstream"
#include "string"
#include "vector"

#define SIZE 10

int func(int i)
{

    std::ofstream fout("./" + std::to_string(i) + ".txt");
    fout << std::to_string(i);
    fout.flush();
    fout.close();

    return i;
}

int main(int argc, char *argv[])
{
    ThrPool pool(SIZE);

    std::vector<ThrPool::Task<int>> a;

    std::vector<int> answers;

    for (size_t i = 0; i < 50; ++i)
    {
        a.push_back(pool.addTask(i, func, i));
    }

    for (auto it = a.begin(); it != a.end(); ++it)
    {
        std::cerr << it->getFutureTask() << "\n";
    }

    std::system("pause");

    return 0;
}
