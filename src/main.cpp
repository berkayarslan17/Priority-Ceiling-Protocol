#include "mutex.h"
#include <iostream>
#include <chrono>

gtu::mutex m1(/*Ceiling Value*/ 10000), m2(/*Ceiling Value*/ 5000);

void threadWork1()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock1(m1);
        std::lock_guard<gtu::mutex> lock2(m2);
    }
}

void threadWork2()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock2(m2);
        std::lock_guard<gtu::mutex> lock1(m1);
    }
}

int main()
{
    std::thread thread1(threadWork1);
    std::thread thread2(threadWork2);

    m1.register_thread(thread1);
    m1.register_thread(thread2);
    m2.register_thread(thread1);
    m2.register_thread(thread2);

    thread1.join();
    thread2.join();

    return 0;
}