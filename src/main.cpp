#include "mutex.h"
#include <iostream>

gtu::mutex m1(10000), m2(5000);

void *threadWork1(void *ptr)
{
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock1(m1);
        printf("Mutex m1 has been taken in thread1\n");
        std::lock_guard<gtu::mutex> lock2(m2);
        printf("Mutex m2 has been taken in thread1\n");
    }
}

void *threadWork2(void *ptr)
{
    for (;;)
    {
        std::lock_guard<gtu::mutex> lock2(m2);
        printf("Mutex m2 has been taken in thread2\n");
        std::lock_guard<gtu::mutex> lock1(m1);
        printf("Mutex m1 has been taken in thread2\n");
    }
}

int main()
{
    pthread_t thread1, thread2;

    /* Create independent threads each of which will execute function */

    // m1.registerThread(thread1);
    // m1.registerThread(thread2);
    // m2.registerThread(thread1);
    // m2.registerThread(thread2);

    pthread_create(&thread1, NULL, threadWork1, NULL);
    pthread_create(&thread2, NULL, threadWork2, NULL);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}