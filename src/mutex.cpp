#include "mutex.h"

thread_local unsigned long gtu::mutex::this_thread_ceiling_value(ULONG_MAX);

void gtu::mutex::lock()
{
    if (check_for_ceiling_violation())
    {
        internal_mutex.lock();
    }

    update_ceiling_value();
}

void gtu::mutex::unlock()
{
    this_thread_ceiling_value = prev_ceiling_value;
    internal_mutex.unlock();
}

bool gtu::mutex::try_lock()
{
    if (check_for_ceiling_violation())
    {
        if (!internal_mutex.try_lock())
        {
            return false;
        }

        update_ceiling_value();
        return true;
    }

    return false;
}

bool gtu::mutex::check_for_ceiling_violation()
{
    if (this_thread_ceiling_value <= ceiling_value)
    {
        return false;
    }

    return true;
}

void gtu::mutex::update_ceiling_value()
{
    prev_ceiling_value = this_thread_ceiling_value;
    this_thread_ceiling_value = ceiling_value;
}

// void gtu::mutex::register_thread(pthread_t thread)
// {
//     sched_param sch;
//     int policy, threadPriority;
//     pthread_getschedparam(thread.native_handle(), &policy, &sch);
//     // Get thread priority for assigning the ceiling value
//     threadPriority = sch.sched_priority;

//     if (ceiling < threadPriority)
//     {
//         ceiling = threadPriority;
//     }
// }