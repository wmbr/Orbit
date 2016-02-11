#ifndef UTIL_HPP_INCLUDED
#define UTIL_HPP_INCLUDED

#include "NBodySystem.hpp"

const int MICROSECONDS_PER_TICK = 200;
const int NUM_BODIES = 4;
const double TIMEDELTA = 0.05;
const double TIMEDELTA_TEST = 0.1;
const int TEST_ITERATIONS = 20000;
const int MAX_TESTS = 1000;
const int TRAIL_FADETIME = 50;


NBodySystem initializeSystem(sf::Vector2u size);

bool valid(const NBodySystem& system, sf::Vector2u size);

bool test(NBodySystem system, sf::Vector2u size);

NBodySystem generateSystem(sf::Vector2u size);

#endif // UTIL_HPP_INCLUDED
