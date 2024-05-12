// AstrophotographyAnalysis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "Executor.h"
#include "Node.h"
#include "Task.h"


class PackageDefault : public fbp::PackageBase
{
public:
    PackageDefault() : PackageBase() { }
    uint32_t data;
};

int main()
{
    using namespace fbp;

    Executor executor;

    std::cout << "program finished with exit code " << 0 << "\n";

    return 0;
}

