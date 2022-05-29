#include "mutex.h"
#include <iostream>
#include <chrono>

gtu::mutex m1(/*Ceiling Value*/ 10000), m2(/*Ceiling Value*/ 5000);

void threadWork1()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto t_start = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay(2000);
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock1(m1);
        std::lock_guard<gtu::mutex> lock2(m2);

        auto t_now = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_start);
        if (elapsed > delay)
        {
            break;
        }
    }
}

void threadWork2()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto t_start = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay(2000);
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock2(m2);
        std::lock_guard<gtu::mutex> lock1(m1);

        auto t_now = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_start);
        if (elapsed > delay)
        {
            break;
        }
    }
}

void test_mutex_avoid_deadlock()
{
    std::thread thread1(threadWork1);
    std::thread thread2(threadWork2);

    m1.register_thread(thread1);
    m1.register_thread(thread2);
    m2.register_thread(thread1);
    m2.register_thread(thread2);

    thread1.join();
    thread2.join();
    std::cout << "Avoid Deadlock test is successful" << std::endl;
}

void test_mutex_register_inviolate()
{
    std::thread thread1(threadWork1);
    std::thread thread2(threadWork2);

    m1.register_thread(thread1);
    m1.register_thread(thread2);
    m2.register_thread(thread1);
    m2.register_thread(thread2);

    thread1.join();
    thread2.join();
    std::cout << "Register Inviolate test is successful" << std::endl;
}

void test_mutex_register_violate()
{
    std::thread thread1(threadWork1);
    std::thread thread2(threadWork2);

    m1.register_thread(thread1);
    m1.register_thread(thread2);
    m2.register_thread(thread1);
    // m2.register_thread(thread2);

    thread1.join();
    thread2.join();
}

int main()
{
    // test_mutex_avoid_deadlock();
    // test_mutex_register_inviolate();
    test_mutex_register_violate();

    return 0;
}