#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include "thrpool.h"
#include "fstream"
#include "logging.h"

using namespace testing;

void blockFunc()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

int returnFunc()
{
    return 10;
}

void writeToFileFunc(int prio)
{
    std::ofstream fout;
    fout.open ("./testPrio.txt", std::ofstream::out | std::ofstream::app);

    fout << std::to_string(prio);

    fout.flush();
    fout.close();
}

TEST(FirstTestCase, FirstTestSet)
{
    ThrPool pool(1);

    EXPECT_EQ(0, pool.mTasks.size());

    ThrPool::Task<void> task0 = pool.addTask(1, blockFunc);

    EXPECT_EQ(1, pool.mTasks.size());

    ThrPool::Task<void> task1 = pool.addTask(2, blockFunc);

    EXPECT_EQ(2, pool.mTasks.size());

    ThrPool::Task<void> task2 = pool.addTask(2, blockFunc);

    EXPECT_EQ(2, pool.mTasks[2].size());

    pool.cancelTask(task2);

    EXPECT_EQ(1, pool.mTasks[2].size());

    ThrPool::Task<int> task3 = pool.addTask(1, returnFunc);

    EXPECT_EQ(std::make_pair(true, 10), pool.getResultOfTask(std::move(task3)));
}

TEST(FirstTestCase, SecondTestSet)
{
    std::ofstream fout("./testPrio.txt");
    fout.flush();
    fout.close();

    ThrPool *pool = new ThrPool(1);

    pool->addTask(1, blockFunc);

    for(int i = 0; i < 5; ++i)
    {
        pool->addTask(1, writeToFileFunc, 1);
        pool->addTask(2, writeToFileFunc, 2);
    }

    delete pool;

    std::string strPrio;
    std::ifstream fin("./testPrio.txt");
    std::string line;
    while ( std::getline(fin, line) )
    {
        strPrio += line;
    }
    fin.close();
    EXPECT_EQ("1111122222", strPrio);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
