#include "mutex.h"
#include "test.h"

int main()
{
    std::cout << "Test cases started" << std::endl;

    test_mutex_avoid_deadlock();
    test_mutex_register_inviolate();
    test_mutex_register_violate();

    return 0;
}