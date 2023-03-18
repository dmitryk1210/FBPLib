// DbgProj_simple_test_main.cpp

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Task.h"
#include "TaskManager.h"


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
    for (uint64_t iteration = 0; iteration < uint64_t(power) * 100; ++iteration)
    {
        temp = std::to_string(data);
        data = std::stoi(temp);
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

    // fbp::TaskManager manager();
    Node node_in("node_input");
    Node node_buf("node_buffer");
    Node node_1("node_1");
    Node node_2("node_2");
    Node node_3("node_3");
    Node node_4("node_4");
    Node node_prt("node_print");
    Node node_out("node_output");

    generateRandomPackagesSet(&node_in, 1000000);

    Task task_read = Task("task_read");
    task_read.setInputNode(&node_in);
    task_read.setOutputNodes(&node_buf);
    task_read.assign(taskReadFunc);
    task_read.start();

    Task task_sort = Task("task_sort");
    task_sort.setInputNode(&node_buf);
    std::vector<fbp::Node*> task_sort_outputs = { &node_1, &node_2, &node_3, &node_4 };
    task_sort.setOutputNodes(task_sort_outputs);
    task_sort.assign(taskSortFunc);
    task_sort.start();

    Task task_dummy1 = Task("task_dummy1");
    task_dummy1.setInputNode(&node_1);
    task_dummy1.setOutputNodes(&node_prt);
    task_dummy1.assign(
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 1);
            targetNode = 0;
            delete packageIn;
        }
    );
    task_dummy1.start();

    Task task_dummy2 = Task("task_dummy2");
    task_dummy2.setInputNode(&node_2);
    task_dummy2.setOutputNodes(&node_prt);
    task_dummy2.assign(
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 2);
            targetNode = 0;
            delete packageIn;
        }
    );
    task_dummy2.start();

    Task task_dummy3 = Task("task_dummy3");
    task_dummy3.setInputNode(&node_3);
    task_dummy3.setOutputNodes(&node_prt);
    task_dummy3.assign(
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 3);
            targetNode = 0;
            delete packageIn;
        }
    );
    task_dummy3.start();

    Task task_dummy4 = Task("task_dummy4");
    task_dummy4.setInputNode(&node_4);
    task_dummy4.setOutputNodes(&node_prt);
    task_dummy4.assign(
        [](fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
        {
            *ppackageOut = new PackageDefault();
            dummy(packageIn, *ppackageOut, 4);
            targetNode = 0;
            delete packageIn;
        }
    );
    task_dummy4.start();

    Task task_print = Task("task_print");
    task_print.setInputNode(&node_prt);
    task_print.setOutputNodes(&node_out);
    task_print.assign(taskPrintFunc);
    task_print.start();

    while (!task_print.isFinalized())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "the end! =)\n";
}

void taskReadFunc(fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
{
    std::stringstream sstream;
    sstream << "read:  " << static_cast<PackageDefault*>(packageIn)->data << "\n";
    //std::cout << sstream.str();

    *ppackageOut = packageIn;
    targetNode = 0;
}

void taskSortFunc(fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
{
    std::stringstream sstream;
    sstream << "sort:  " << static_cast<PackageDefault*>(packageIn)->data << "\n";
    //std::cout << sstream.str();

    *ppackageOut = packageIn;
    targetNode = static_cast<PackageDefault*>(packageIn)->data % 4;
}

void taskPrintFunc(fbp::PackageBase* packageIn, fbp::PackageBase** ppackageOut, int& targetNode)
{
    std::stringstream sstream;
    sstream << "print: " << static_cast<PackageDefault*>(packageIn)->data << "\n";
    //std::cout << sstream.str();

    *ppackageOut = packageIn;
    targetNode = 0;
}
