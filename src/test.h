#ifndef _TEST_H_
#define _TEST_H_

#include <thread>
#include <iostream>
#include <chrono>
#include "mutex.h"

void test_mutex_avoid_deadlock();
void test_mutex_register_inviolate();
void test_mutex_register_violate();

#endif