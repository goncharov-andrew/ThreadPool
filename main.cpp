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

    //_sleep(rand() / 100 + 100);

    return i;
}

int main(int argc, char *argv[])
{
    ThrPool pool(SIZE);

    std::vector<ThrPool::Task<int>> a;

    std::vector<int> answers;

    for (size_t i = 0; i < 90; ++i)
    {
        a.push_back(pool.addTask(i, func, i));
    }

    //auto t0 = pool.addTask(0, func, 20);
    //auto t1 = pool.addTask(1, func, 21);
    //auto t2 = pool.addTask(0, func, 22);
    //auto t3 = pool.addTask(1, func, 23);
    //auto t4 = pool.addTask(2, func, 24);
    //auto t5 = pool.addTask(1, func, 25);
    //auto t6 = pool.addTask(3, func, 26);
    //auto t7 = pool.addTask(2, func, 27);
    //auto t8 = pool.addTask(1, func, 28);
    //auto t9 = pool.addTask(0, func, 29);

    for (auto it = a.begin(); it != a.end(); ++it)
    {

        std::cerr << it->getFutureTask() << "\n";
    }

    std::system("pause");

    return 0;
}
