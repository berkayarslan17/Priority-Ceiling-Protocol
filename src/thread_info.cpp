#include "thread_info.h"

int get_thread_priority(std::thread &thread)
{
    sched_param sch;
    int policy, thread_priority, ret;
    ret = pthread_getschedparam(thread.native_handle(), &policy, &sch);
    // // Get thread priority for assigning the ceiling value
    thread_priority = sch.sched_priority;
    return thread_priority;
}

void set_thread_priority(std::thread &thread, int priority)
{
    sched_param sch;
    int policy;
    sch.sched_priority = priority;
    if (pthread_setschedparam(thread.native_handle(), policy, &sch))
    {
        std::cout << "Failed to set thread scheduling :" << std::endl;
    }
}