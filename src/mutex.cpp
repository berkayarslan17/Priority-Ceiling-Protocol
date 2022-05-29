#include "mutex.h"

thread_local unsigned long gtu::mutex::this_thread_ceiling_value(ULONG_MAX);

static inline int get_thread_priority(std::thread &thread);

void gtu::mutex::lock()
{
    if (check_for_ceiling_violation() && check_for_registration_violation())
    {
        internal_mutex.lock();
        update_ceiling_value();
    }
}

void gtu::mutex::unlock()
{
    this_thread_ceiling_value = prev_ceiling_value;
    internal_mutex.unlock();
}

bool gtu::mutex::try_lock()
{
    if (check_for_ceiling_violation() && check_for_registration_violation())
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

bool gtu::mutex::check_for_registration_violation()
{
    int status = THREAD_NOT_FOUND;

    if (!threads_info.empty())
    {
        // Search the threads_info vector
        for (auto it = threads_info.begin(); it != threads_info.end(); it++)
        {
            // If the system finds the thread in thread info, return true.
            if (it->id == std::this_thread::get_id())
            {
                // printf("thread registered\n");
                status = THREAD_FOUND;
                return status;
            }
        }

        // If thread_info is filled but specific thread is not found, throw an exception.
        throw std::logic_error("mutex registration violated");
    }

    // If thread_info is empty, throw an exception.
    else
    {
        throw std::logic_error("mutex registration violated");
    }
}

void gtu::mutex::update_ceiling_value()
{
    prev_ceiling_value = this_thread_ceiling_value;
    this_thread_ceiling_value = ceiling_value;
}

void gtu::mutex::register_thread(std::thread &thread)
{
    thread_info_t this_thread_info;

    // (void)get_thread_priority(thread);
    this_thread_info.priority = get_thread_priority(thread);
    std::cout << "priority is " << this_thread_info.priority << std::endl;
    this_thread_info.id = thread.get_id();
    std::cout << "thread id is " << this_thread_info.id << std::endl;

    threads_info.push_back(this_thread_info);
}

static inline int get_thread_priority(std::thread &thread)
{
    sched_param sch;
    int policy, thread_priority, ret;
    ret = pthread_getschedparam(thread.native_handle(), &policy, &sch);
    // // Get thread priority for assigning the ceiling value
    thread_priority = sch.sched_priority;
    return thread_priority;
}