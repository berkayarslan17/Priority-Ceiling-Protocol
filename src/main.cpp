#include "mutex.h"
#include <iostream>
#include <chrono>

gtu::mutex m1(/*mutex_id*/ 1, /*ceiling_value*/ 10000), m2(/*mutex_id*/ 2, /*ceiling_value*/ 5000);

void thread_work1()
{
    for (;;)
    {
        std::cout << "Deneme 1" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<gtu::mutex> lock1(m1);
        std::cout << "Deneme 2" << std::endl;
        std::lock_guard<gtu::mutex> lock2(m2);
        std::cout << "Deneme 3" << std::endl;
    }
}

void thread_work2()
{
    for (;;)
    {
        std::cout << "Deneme 4" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<gtu::mutex> lock2(m2);
        std::cout << "Deneme 5" << std::endl;
        std::lock_guard<gtu::mutex> lock1(m1);
        std::cout << "Deneme 6" << std::endl;
    }
}

int main()
{
    std::thread thread1(thread_work1);
    // set_thread_priority(thread1, 3);
    std::thread thread2(thread_work2);
    // set_thread_priority(thread2, 5);

    // Write unit test for registration violation
    m1.register_thread(thread1);
    m1.register_thread(thread2);
    m2.register_thread(thread1);
    m2.register_thread(thread2);

    thread1.join();
    thread2.join();

    return 0;
}