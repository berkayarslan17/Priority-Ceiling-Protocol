#include "test.h"

gtu::mutex m1(/*Ceiling Value*/ 10000), m2(/*Ceiling Value*/ 5000);
gtu::mutex m3(/*Ceiling Value*/ 10000), m4(/*Ceiling Value*/ 5000);

static void threadWork1();
static void threadWork2();
static void threadWork3();
static void threadWork4();

void test_mutex_avoid_deadlock()
{
    std::cout << "Avoid deadlock test... " << std::endl;
    std::thread thread1(threadWork1);
    std::thread thread2(threadWork2);

    m1.register_thread(thread1);
    m1.register_thread(thread2);
    m2.register_thread(thread1);
    m2.register_thread(thread2);

    thread1.join();
    thread2.join();
    std::cout << "SUCCESSFUL" << std::endl;
}

void test_mutex_register_inviolate()
{
    std::cout << "Register inviolate test... " << std::endl;
    std::thread thread1(threadWork1);
    std::thread thread2(threadWork2);

    m1.register_thread(thread1);
    m1.register_thread(thread2);
    m2.register_thread(thread1);
    m2.register_thread(thread2);

    thread1.join();
    thread2.join();
    std::cout << "SUCCESSFUL" << std::endl;
}

void test_mutex_register_violate()
{
    std::cout << "Register violate test... " << std::endl;
    std::thread thread1(threadWork3);
    std::thread thread2(threadWork4);

    m1.register_thread(thread1);
    m1.register_thread(thread2);
    // m2.register_thread(thread1);
    // m2.register_thread(thread2);

    thread1.join();
    thread2.join();
    std::cout << "FAILED" << std::endl;
}

static void threadWork1()
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

static void threadWork2()
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

static void threadWork3()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto t_start = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay(2000);
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock3(m3);
        std::lock_guard<gtu::mutex> lock4(m4);

        auto t_now = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_start);
        if (elapsed > delay)
        {
            break;
        }
    }
}

static void threadWork4()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto t_start = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay(2000);
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock4(m4);
        std::lock_guard<gtu::mutex> lock3(m3);

        auto t_now = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_start);
        if (elapsed > delay)
        {
            break;
        }
    }
}