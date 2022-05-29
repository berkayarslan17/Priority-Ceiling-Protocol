#include "mutex.h"

#define INVALID_INDEX -1

thread_local unsigned long gtu::mutex::this_thread_ceiling_value(ULONG_MAX);

void gtu::mutex::lock()
{
    std::thread::id this_thread_id = std::this_thread::get_id();

    if (check_for_ceiling_violation() && check_for_registration_violation())
    {
        internal_mutex.lock();
        // Change thread state to LOCK_STATE
        event_t evt = {.sig = THREAD_SIG_LOCK};
        thread_dispatch(this_thread_id, &evt);

        update_ceiling_value();
    }
}

void gtu::mutex::unlock()
{
    std::thread::id this_thread_id = std::this_thread::get_id();

    this_thread_ceiling_value = prev_ceiling_value;
    internal_mutex.unlock();
    // Change thread state to UNLOCK_STATE
    event_t evt = {.sig = THREAD_SIG_UNLOCK};
    thread_dispatch(this_thread_id, &evt);
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
    std::thread::id this_thread_id = std::this_thread::get_id();

    if (this_thread_ceiling_value <= ceiling_value)
    {
        return false;
        // Change thread state to WAIT_STATE and inherit its priority
        event_t evt = {.sig = THREAD_SIG_WAIT};
        thread_dispatch(this_thread_id, &evt);
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
                printf("thread registered\n");
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

    this_thread_info.priority = get_thread_priority(thread);
    this_thread_info.id = thread.get_id();
    this_thread_info.state = THREAD_STATE_INITIAL;
    this_thread_info.mutex_id = mutex_id;
    this_thread_info.thread = std::move(thread);

    threads_info.push_back(std::move(this_thread_info));
}

int gtu::mutex::find_thread_by_id(std::thread::id thread_id)
{
    int index;

    auto it = find_if(threads_info.begin(), threads_info.end(), [&thread_id](const thread_info &me)
                      { return me.id == thread_id; });

    if (it != threads_info.end())
    {
        index = std::distance(threads_info.begin(), it);
    }
    else
    {
        std::cout << "There is no thread with this id" << std::endl;
        index = INVALID_INDEX;
    }

    return index;
}

void gtu::mutex::thread_dispatch(std::thread::id thread_id, event_t *evt)
{
    int this_thread_index = find_thread_by_id(thread_id);

    switch (threads_info[this_thread_index].state)
    {
    case THREAD_STATE_INITIAL:
    {
        switch (evt->sig)
        {
        case THREAD_SIG_LOCK:
        {
            threads_info[this_thread_index].state = THREAD_STATE_LOCK;
            break;
        }

        case THREAD_SIG_WAIT:
        {
            threads_info[this_thread_index].state = THREAD_STATE_WAIT;
            int mutex_id = threads_info[this_thread_index].mutex_id;
            std::thread::id lock_thread_id = get_lock_thread_id(mutex_id);

            int lock_thread_index = find_thread_by_id(lock_thread_id);

            if (lock_thread_index != INVALID_INDEX)
            {
                // If lock thread's priority is smaller than wait thread, inherit its priority.
                if (threads_info[this_thread_index].priority > threads_info[lock_thread_index].priority)
                {
                    // set_thread_priority(threads_info[lock_thread_index].thread, threads_info[this_thread_index].priority);
                }
            }

            break;
        }
        default:
        {
            std::cout << "Unknown event" << evt->sig << std::endl;
            break;
        }
        }
        break;
    }

    case THREAD_STATE_LOCK:
    {
        switch (evt->sig)
        {
        case THREAD_SIG_UNLOCK:
        {
            threads_info[this_thread_index].state = THREAD_STATE_UNLOCK;
            // Always set priority to normal in case of inheriting other threads priorities.
            // set_thread_priority(threads_info[this_thread_index].thread, threads_info[this_thread_index].priority);
            break;
        }

            // case THREAD_SIG_WAIT:
            // {
            //     threads_info[this_thread_index].state = THREAD_STATE_WAIT;
            //     int mutex_id = threads_info[this_thread_index].mutex_id;
            //     std::thread::id lock_thread_id = get_lock_thread_id(mutex_id);

            //     int lock_thread_index = find_thread_by_id(lock_thread_id);

            //     if (lock_thread_index != INVALID_INDEX)
            //     {
            //         // If lock thread's priority is smaller than wait thread, inherit its priority.
            //         if (threads_info[this_thread_index].priority > threads_info[lock_thread_index].priority)
            //         {
            //             set_thread_priority(threads_info[lock_thread_index].thread, threads_info[this_thread_index].priority);
            //         }
            //     }

            //     break;
            // }

        default:
        {
            std::cout << "Unknown event " << evt->sig << std::endl;
            break;
        }
        }
        break;
    }

    case THREAD_STATE_UNLOCK:
    {
        switch (evt->sig)
        {
        case THREAD_SIG_LOCK:
        {
            threads_info[this_thread_index].state = THREAD_STATE_LOCK;
            break;
        }

        case THREAD_SIG_WAIT:
        {
            threads_info[this_thread_index].state = THREAD_STATE_WAIT;
            int mutex_id = threads_info[this_thread_index].mutex_id;
            std::thread::id lock_thread_id = get_lock_thread_id(mutex_id);

            int lock_thread_index = find_thread_by_id(lock_thread_id);

            if (lock_thread_index != INVALID_INDEX)
            {
                // If lock thread's priority is smaller than wait thread, inherit its priority.
                if (threads_info[this_thread_index].priority > threads_info[lock_thread_index].priority)
                {
                    // set_thread_priority(threads_info[lock_thread_index].thread, threads_info[this_thread_index].priority);
                }
            }

            break;
        }
        default:
        {
            std::cout << "Unknown event " << evt->sig << std::endl;
            break;
        }
        }
        break;

    case THREAD_STATE_WAIT:
    {
        switch (evt->sig)
        {
        // If the beacon is inside, switch to IN_RANGE state
        case THREAD_STATE_LOCK:
        {
            threads_info[this_thread_index].state = THREAD_STATE_LOCK;
            break;
        }

        default:
        {
            std::cout << "Unknown event " << evt->sig << std::endl;
            break;
        }
        }
        break;
    }
    }
    }
}

std::thread::id gtu::mutex::get_lock_thread_id(int mutex_id)
{
    std::thread::id dump_id;
    // Search the threads_info vector
    for (auto it = threads_info.begin(); it != threads_info.end(); it++)
    {
        // If the system finds the thread in thread info, return true.
        if (it->state == THREAD_STATE_LOCK && it->mutex_id == mutex_id)
        {
            return it->id;
        }
    }

    return dump_id;
}