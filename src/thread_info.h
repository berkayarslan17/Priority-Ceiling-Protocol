#ifndef _THREAD_INFO_H_
#define _THREAD_INFO_H_

#include <iostream>
#include <pthread.h>
#include <thread>
#include <mutex>

#define THREAD_FOUND 1
#define THREAD_NOT_FOUND 0

typedef enum thread_signals
{

    /** @brief THREAD_SIG_PRIO_INHERIT:
     */
    THREAD_SIG_LOCK,
    /** @brief THREAD_SIG_PRIO_INHERIT:
     */
    THREAD_SIG_UNLOCK,
    /** @brief THREAD_SIG_PRIO_INHERIT:
     */
    THREAD_SIG_WAIT,
    /** @brief THREAD_SIG_PRIO_INHERIT:
     */
    THREAD_SIG_PRIO_INHERIT,
} thread_sig_t;

typedef enum thread_states
{
    /** @brief INITIAL_STATE:
     *
     */
    THREAD_STATE_INITIAL,

    /** @brief LOCK_STATE:
     *
     */
    THREAD_STATE_LOCK,

    /** @brief UNLOCK_STATE:
     *
     */
    THREAD_STATE_UNLOCK,

    /** @brief WAIT_STATE:
     *
     */
    THREAD_STATE_WAIT,
} thread_state_t;

/**
 * @brief
 *
 */
typedef struct event_tag
{
    thread_sig_t sig;
} event_t;

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
    std::thread thread;
    std::thread::id id;
    thread_state_t state;
    int priority;
    int mutex_id;
} thread_info_t;

int get_thread_priority(std::thread &thread);
void set_thread_priority(std::thread &thread, int priority);

#endif