#ifndef _MUTEX_H
#define _MUTEX_H

#include <iostream>
#include <mutex>
#include <climits>
#include <vector>
#include <pthread.h>
#include <thread>

#define THREAD_FOUND 1
#define THREAD_NOT_FOUND 0

/**
 * @todo
 *
 * 1- (DONE) gtu::mutex shall implement std::mutex interface that is required by std::lock_guard and std::unique_lock.
 *
 * 2- (DONE) gtu::mutex shall provide an interface to register threads that are capable of locking this mutex in advance. During
 *    runtime, if a thread tries to lock the mutex, but hasn’t registered itself before, a runtime exception shall be raised.
 *
 * 3- (DONE) The designed mutex shall be able to avoid deadlocks.
 *
 * 4- (IN PROGRESS) If a lock attempt is blocked due to priority ceiling protocol rule (that is, the thread trying to lock does not have
 *    higher priority than the highest ceiling of all acquired locks), the priority of the thread holding the lock will be
 *    elevated if necessary.
 *
 */
namespace gtu
{
    /**
     * @brief A Thread Info Struct
     *
     * This structure keeps information related to each thread that registers to gtu::mutex class.
     * Information of threads are important for a couple of things. One of the first thing is checking
     * whether a violation exists or not. Second thing is we have to know the threads priority in order to
     * inherit their priorities properly.
     *
     */
    typedef struct thread_info
    {
        int priority;
        std::thread::id id;
    } thread_info_t;

    /**
     * @brief A Custom Mutex Class
     *
     * This class exists primarily to overcome an issue that affects real time performance of C++11
     * processes. It implements Priority Ceiling Protocol structure in order to avoid some problems
     * in Real-Time Systems like deadlock or Priority Inversion. Priority Ceiling Protocol is a thread
     * synchronization protocol that is better than Priority Inheritance protocol in many ways.
     *
     * In Priority Ceiling Protocol an assumption is made that all the threads in the system have a fixed
     * priority. It does not fall into a deadlock state.
     *
     */

    class mutex
    {
    private:
        /**
         * @brief The Threads Info Vector
         *
         * This vector keeps all the data related to each thread in itself.
         *
         */
        std::vector<thread_info_t> threads_info;

        /**
         * @brief The Mutex
         *
         * This is the non-pthreads default C++11 std::mutex wrapped by this class
         *
         */
        std::mutex internal_mutex;

        /**
         * @brief Ceiling Value of Mutex
         *
         * It indicates the ceiling value of the mutex.
         *
         */
        unsigned int const ceiling_value;

        /**
         * @brief Previous Ceiling Value of Mutex
         *
         * It's important to save the previous value of the ceiling value for the current thread so we can restroe it in
         * unlock(), otherwise you'd never be able to lock a mutex with a higher ceiling value again, even if the thread didn't
         * hold any locks.
         *
         */
        unsigned int prev_ceiling_value;

        /**
         * @brief Ceiling value of the Thread
         *
         * The use of the thread_local value representing the ceiling value for the current thread. Because it is
         * declared thread_local, every thread has its own copy, so the state of the variable in one thread is entirely
         * independent of the state of the variable when read from another thread.
         *
         */
        static thread_local unsigned long this_thread_ceiling_value;

        /**
         * @brief Ceiling Violation Checking Operation
         *
         * A Thread, can acquire a lock only if the thread's priority is strictly greater than the priority ceilings of
         * all the locks held by other threads. This function checks whether the thread's ceiling value is greater than
         * the ceiling value of the mutex.
         *
         * @return true  If thread's ceiling value bigger than ceiling value of mutex.
         * @return false If thread's ceiling value smaller than ceiling value of mutex.
         */
        bool check_for_ceiling_violation();

        /**
         * @brief Registration Violation Checking Operation
         *
         * gtu::mutex provides an interface to register threads that are capable of locking this mutex in advance. During
         * runtime, if a thread tries to lock the mutex, but hasn’t registered itself before, a runtime exception is raised.
         *
         * @throw Throws std::logic error if the thread hasn't registered mutex before.
         */
        bool check_for_registration_violation();

        /**
         * @brief Ceiling Update Operation
         *
         * If the thread acquires the lock, our mutex needs to update the thread's ceiling value and save the previous
         * ceiling value.
         *
         */
        void update_ceiling_value();

    public:
        /**
         * @brief Qualified Constructor for Mutex
         *
         * @param value Priority Ceiling Value of Mutex
         */
        explicit mutex(unsigned long value) : ceiling_value(value),
                                              prev_ceiling_value(0)
        {
        }

        /**
         * @brief Registration of Thread
         *
         * This function saves related thread info to gtu::mutex::threads_info vector in order to detect registration
         * violation.
         *
         * @param thread related thread
         */
        void register_thread(std::thread &thread);

        /**
         * @brief The Lock Operation
         *
         * If there is no violation in the priority, thread can acquire the lock and update it's ceiling value.
         *
         */
        void lock();

        /**
         * @brief The Unlock Operation
         *
         * Thread restores the previous ceiling value and releases the lock.
         *
         */
        void unlock();

        /**
         * @brief The Try Lock Operation
         *
         * If there is no violation in the priority, thread tries to acquire the lock and update it's ceiling value.
         *
         * @return true If the mutex was successfully locked and false otherwisee
         */
        bool try_lock();
    };

}

#endif