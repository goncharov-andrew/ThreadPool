#include <iostream>
#include "thrpool.h"
#include "fstream"
#include "string"
#include "vector"
#include <iterator>

#include "logging.h"

#define NUMBER_OF_THREADS 4
#define NUMBER_OF_TASKS   10000

int* TestMas = new int[NUMBER_OF_TASKS];
bool flag = false;

int testStressFunc(int i)
{
    while(false == flag)
    {}

    TestMas[i] = i;

    std::this_thread::sleep_for(std::chrono::milliseconds((1 + (rand() % 100))));

    return i;
}

int main(int argc, char *argv[])
{
    ThrPool pool(NUMBER_OF_THREADS);

    std::vector<double> timeResults;

    for(int z = 0; z < 10; ++z)
    {
        std::vector<ThrPool::Task<int>> a;

        auto t1 = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < NUMBER_OF_TASKS; ++i)
        {
            a.push_back(pool.addTask(1, testStressFunc, i));//rand() % 5
        }

        flag = true;

        /*for(size_t i = 0; i < NUMBER_OF_TASKS / 10; ++i)
        {
            auto it =  a.begin() + (rand() % (NUMBER_OF_TASKS - 1));
            pool.cancelTask(*it);
        }*/

        for (auto it = a.begin(); it != a.end(); ++it)
        {
            std::pair<bool, int> resultOfTask = pool.getResultOfTask(std::move(*it));

            if(true == resultOfTask.first)
            {
                std::cerr << resultOfTask.second << std::endl;
            }
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

        timeResults.push_back(fp_ms.count());

        flag = false;
    }

    std::ofstream output_file("./results.txt");
    std::ostream_iterator<double> output_iterator(output_file, "\n");
    std::copy(timeResults.begin(), timeResults.end(), output_iterator);

    return 0;
}
