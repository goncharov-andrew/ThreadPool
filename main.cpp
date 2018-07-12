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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return i;
}

int main(int argc, char *argv[])
{
    ThrPool pool(SIZE);

    std::vector<ThrPool::Task<int>> a;

    for (size_t i = 0; i < 50; ++i)
    {
        a.push_back(pool.addTask(i, func, i));
    }

    pool.cancelTask(48);

    for (auto it = a.begin(); it != a.end(); ++it)
    {
        std::cerr << it->getFutureTask() << "\n";
    }

    std::system("pause");

    return 0;
}
