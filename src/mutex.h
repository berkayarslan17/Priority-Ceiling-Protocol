#ifndef _MUTEX_H
#define _MUTEX_H

#include <mutex>
#include <climits>
#include <pthread.h>

namespace gtu
{
    class mutex
    {
    private:
        std::mutex internal_mutex;
        unsigned int const ceiling_value;
        unsigned int prev_ceiling_value;
        static thread_local unsigned long this_thread_ceiling_value;

        bool check_for_ceiling_violation();
        void update_ceiling_value();

    public:
        explicit mutex(unsigned long value) : ceiling_value(value),
                                              prev_ceiling_value(0)
        {
        }

        void register_thread(pthread_t thread);
        void lock();
        void unlock();
        bool try_lock();
    };

}

#endif