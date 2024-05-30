// TracesExtraction.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Executor.h"
#include "Node.h"
#include "Task.h"


class PackageDefault : public fbp::PackageBase
{
public:
    PackageDefault() : PackageBase() { }
};


int main()
{
    using namespace fbp;

    Executor executor;

    auto start = std::chrono::high_resolution_clock::now();


    executor.Execute(true);

    while (!executor.IsDone())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    executor.PrintDebugData("debugData.out");
    executor.Terminate();
    std::cout << "the end!\n";

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "time: " << duration.count() << " ms\n";

    return 0;
}
