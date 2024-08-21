#include <iostream>
#include "ThreadPool.h"
#include <chrono>

class Test
{
public:
    Test()
    {
        std::cout << "Constructor\n";
    }

    Test(const Test &test)
    {
        std::cout << "Copy constructor\n";
    }

    Test(Test &&test) noexcept
    {
        std::cout << "Move constructor\n";
    }

    Test& operator=(const Test &other)
    {
        std::cout << "Copy assignment operator\n";
        return *this;
    }

    Test& operator=(Test &&other) noexcept
    {
        std::cout << "Move assignment operator\n";
        return *this;
    }

    ~Test()
    {
        std::cout << "Destructor\n";
    }
};

void test_no_unnecessary_copies()
{
    ConcurrentQueue<Test> cq;
    cq.push(Test());
    std::cout << std::endl;
    Test test;
    auto res = cq.try_pop();
}

void test_basic_pool()
{
    std::cout << std::thread::hardware_concurrency() << std::endl;
    ThreadPool thp(5);

    auto task1 = []() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        static std::atomic<int> counter = 0;
        std::cout << "Finished work for task " << counter << ".\n";
        counter++;
    };

    for (int i = 0; i < 5; ++i)
        thp.submit(task1);

    std::this_thread::sleep_for(std::chrono::seconds(8));
    thp.shutdown();
}

int main()
{
    test_no_unnecessary_copies();
    test_basic_pool();
    return 0;
}
