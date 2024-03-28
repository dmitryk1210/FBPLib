// DbgProj_simple_test_main.cpp

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
    uint32_t data;
};

void dummy(fbp::PackageBase* packageBaseIn, fbp::PackageBase* packageBaseOut, int power)
{
    PackageDefault* packageIn = static_cast<PackageDefault*>(packageBaseIn);
    PackageDefault* packageOut = static_cast<PackageDefault*>(packageBaseOut);
    uint32_t data = packageIn->data;
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
        PackageDefault* pack = new PackageDefault();
        pack->data = std::rand();
        ++generatedCount;
        node->push(pack);

        //std::stringstream ss;
        //ss << "generated pack " << pack->data << " \n";
        //std::string sstring = ss.str();
        //std::cout << sstring;
    }
    node->push(new fbp::PackageEndOfStream());
    std::cout << "generate " << count << " packages\n";
}

void taskReadFunc(fbp::PackageBase*, fbp::PackageBase**, int&);
void taskSortFunc(fbp::PackageBase*, fbp::PackageBase**, int&);
void taskPrintFunc(fbp::PackageBase*, fbp::PackageBase**, int&);


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

    executor.addTask("task_read", &node_in, &node_buf, taskReadFunc);

    std::vector<fbp::Node*> task_sort_outputs = { &node_1, &node_2, &node_3, &node_4 };
    executor.addTask("task_sort", &node_buf, task_sort_outputs, taskSortFunc);

    executor.addTask("task_dummy1", &node_1, &node_prt,
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 1);
            targetNode = 0;
            delete packageIn;
        }
    );

    executor.addTask("task_dummy2", &node_2, &node_prt,
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 2);
            targetNode = 0;
            delete packageIn;
        }
    );

    executor.addTask("task_dummy3", &node_3, &node_prt,
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 3);
            targetNode = 0;
            delete packageIn;
        }
    );

    executor.addTask("task_dummy4", &node_4, &node_prt,
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 4);
            targetNode = 0;
            delete packageIn;
        }
    );

    executor.addTask("task_print", &node_prt, &node_out, taskPrintFunc);

    executor.execute();

    while (!executor.isDone())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    executor.terminate();
    std::cout << "the end! =)\n";

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "time: " << duration.count() << " ms\n";

    return 0;
}

void taskReadFunc(fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
{
    //std::stringstream sstream;
    //sstream << "read:  " << static_cast<PackageDefault*>(packageIn)->data << "\n";
    //std::cout << sstream.str();

    *ppackageOut = packageIn;
    targetNode = 0;
}

void taskSortFunc(fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
{
    //std::stringstream sstream;
    //sstream << "sort:  " << static_cast<PackageDefault*>(packageIn)->data << "\n";
    //std::cout << sstream.str();

    *ppackageOut = packageIn;
    targetNode = static_cast<PackageDefault*>(packageIn)->data % 4;
}

void taskPrintFunc(fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
{
    //std::stringstream sstream;
    //sstream << "print: " << static_cast<PackageDefault*>(packageIn)->data << "\n";
    //std::cout << sstream.str();

    delete packageIn;
    *ppackageOut = nullptr;
    targetNode = 0;
}
