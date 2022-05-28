#include "mutex.h"
#include <iostream>
#include <chrono>

gtu::mutex m1(10000), m2(5000);

void threadWork1()
{
    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<gtu::mutex> lock1(m1);
        // printf("Mutex m1 has been taken in thread1\n");
        std::lock_guard<gtu::mutex> lock2(m2);
        // printf("Mutex m2 has been taken in thread1\n");
    }
}

void threadWork2()
{
    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<gtu::mutex> lock2(m2);
        // printf("Mutex m2 has been taken in thread2\n");
        std::lock_guard<gtu::mutex> lock1(m1);
        // printf("Mutex m1 has been taken in thread2\n");
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