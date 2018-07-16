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

void vFunc()
{
    std::ofstream fout("./" + std::to_string(999) + ".txt");
    fout << std::to_string(999);
    fout.flush();
    fout.close();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main(int argc, char *argv[])
{
    ThrPool pool(SIZE);

    std::vector<ThrPool::Task<int>> a;

    for (size_t i = 0; i < 50; ++i)
    {
        a.push_back(pool.addTask(i, func, i));
    }

    std::cerr << pool.cancelTask(a[48]) << std::endl;

    for (auto it = a.begin(); it != a.end(); ++it)
    {
        std::pair<bool, int> resultOfTask = pool.getResultOfTask(std::move(*it));

        if(true == resultOfTask.first)
        {
            std::cerr << resultOfTask.second << std::endl;
        }
    }

    pool.addTask(1, vFunc);

    std::system("pause");

    return 0;
}
