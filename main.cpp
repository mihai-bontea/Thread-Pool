#include <iostream>
#include "ConcurrentQueue.h"

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

void test_no_unecessary_copies()
{
    ConcurrentQueue<Test> cq;
    cq.push(Test());
    std::cout << std::endl;
    Test test;
    cq.try_pop(test);
}

int main()
{
    test_no_unecessary_copies();
    return 0;
}
