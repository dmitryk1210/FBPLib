// DbgProj_simple_test_main.cpp

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "fbplib.h"


struct PackageDefault : public fbp::PackageBase
{
    PackageDefault() : PackageBase() { }
    uint32_t data;
};

void dummy(std::unique_ptr<fbp::PackageBase> packageBaseIn, fbp::PackageBase* packageBaseOut, int power)
{
    PackageDefault* packageOut = static_cast<PackageDefault*>(packageBaseOut);
    uint32_t data = static_cast<PackageDefault*>(packageBaseIn.get())->data;
    std::string temp;
    for (uint64_t iteration = 0; iteration < uint64_t(5 - power) * 1000; ++iteration)
    {
        temp = std::to_string(data);
        data = std::stoi(temp) + 1;
    }
    packageOut->data = data;
}

void generateRandomPackagesSet(fbp::Node* node, int count)
{
    int generatedCount = 0;
    while (generatedCount < count)
    {
        std::unique_ptr<PackageDefault> pack = std::make_unique<PackageDefault>();
        pack->data = std::rand();
        ++generatedCount;
        node->Push(std::move(pack));
    }
    node->Push(std::make_unique<fbp::PackageEndOfStream>());
    std::cout << "generate " << count << " packages\n";
}

void taskReadFunc(std::unique_ptr<fbp::PackageBase>,  fbp::Task*);
void taskSortFunc(std::unique_ptr<fbp::PackageBase>,  fbp::Task*);
void taskPrintFunc(std::unique_ptr<fbp::PackageBase>, fbp::Task*);


int main()
{
    using namespace fbp;

    Executor executor;

    auto start = std::chrono::high_resolution_clock::now();

    Node node_in("node_input");
    Node node_buf("node_buffer");
    Node node_1("node_1");
    Node node_2("node_2");
    Node node_3("node_3");
    Node node_4("node_4");
    Node node_prt("node_print");
    Node node_out("node_output");

    generateRandomPackagesSet(&node_in, 1000000);

    executor.AddTask("task_read", &node_in, &node_buf, taskReadFunc);

    std::vector<fbp::Node*> task_sort_outputs = { &node_1, &node_2, &node_3, &node_4 };
    executor.AddTask("task_sort", &node_buf, task_sort_outputs, taskSortFunc);

    executor.AddTask("task_dummy1", &node_1, &node_prt,
        [](std::unique_ptr<fbp::PackageBase> packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<fbp::PackageBase> packageOut = std::make_unique<PackageDefault>();
            dummy(std::move(packageIn), packageOut.get(), 1);
            
            pTask->GetOutputNode()->Push(std::move(packageOut));
        }
    );

    executor.AddTask("task_dummy2", &node_2, &node_prt,
        [](std::unique_ptr<fbp::PackageBase> packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageDefault> packageOut = std::make_unique<PackageDefault>();
            dummy(std::move(packageIn), packageOut.get(), 2);

            pTask->GetOutputNode()->Push(std::move(packageOut));
        }
    );

    executor.AddTask("task_dummy3", &node_3, &node_prt,
        [](std::unique_ptr<fbp::PackageBase> packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageDefault> packageOut = std::make_unique<PackageDefault>();
            dummy(std::move(packageIn), packageOut.get(), 3);

            pTask->GetOutputNode()->Push(std::move(packageOut));
        }
    );

    executor.AddTask("task_dummy4", &node_4, &node_prt,
        [](std::unique_ptr<fbp::PackageBase> packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageDefault> packageOut = std::make_unique<PackageDefault>();
            dummy(std::move(packageIn), packageOut.get(), 4);

            pTask->GetOutputNode()->Push(std::move(packageOut));
        }
    );

    executor.AddTask("task_print", &node_prt, &node_out, taskPrintFunc);

    executor.ExecuteAndAwait();
    executor.PrintDebugData("debugData.out");
    executor.Terminate();
    std::cout << "the end! =)\n";

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "time: " << duration.count() << " ms\n";

    return 0;
}

void taskReadFunc(std::unique_ptr<fbp::PackageBase> packageIn, fbp::Task* pTask)
{
    pTask->GetOutputNode()->Push(std::move(packageIn));
}

void taskSortFunc(std::unique_ptr<fbp::PackageBase> packageIn, fbp::Task* pTask)
{
    int nodeId = static_cast<PackageDefault*>(packageIn.get())->data % 4;
    std::string nameOutputNode = "";
    switch (nodeId) {
    case 0:
        nameOutputNode = "node_1";
        break;
    case 1:
        nameOutputNode = "node_2";
        break;
    case 2:
        nameOutputNode = "node_3";
        break;
    default:
        nameOutputNode = "node_4";
        break;
    }
    pTask->GetOutputNode(nameOutputNode)->Push(std::move(packageIn));
}

void taskPrintFunc(std::unique_ptr<fbp::PackageBase> packageIn, fbp::Task* pTask)
{
}
