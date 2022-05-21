#ifndef _MUTEX_H
#define _MUTEX_H

#include <mutex>

namespace gtu
{
    class mutex
    {
    private:
        std::mutex mutex;
    };

};

#endif