#ifndef UTIL_HPP_INCLUDED
#define UTIL_HPP_INCLUDED

#include "NBodySystem.hpp"

const int MICROSECONDS_PER_TICK = 200;
const int NUM_BODIES = 4;
const double TIMEDELTA = 0.05;
const double TIMEDELTA_TEST = 0.5;
const int TEST_ITERATIONS = 10000;
const int MAX_TESTS = 1000;


NBodySystem initializeSystem(int width, int height);

bool valid(const NBodySystem& system, int width, int height);

bool test(NBodySystem system, int width, int height);

NBodySystem generateSystem(int width, int height);

#endif // UTIL_HPP_INCLUDED
