#include "util.hpp"

NBodySystem initializeSystem(int width, int height)
{
	NBodySystem system;
	std::vector<Body> bodies;
	for(int i = 0; i < NUM_BODIES; ++i)
	{
		double radius = std::rand() % 12 + 3;
		double x = std::rand() % (width/2) + (width/4.0);
		double y = std::rand() % (height/2) + (height/4.0);
		double vx = (std::rand() % 100) / 100.0;
		double vy = (std::rand() % 100) / 100.0;
		sf::Color color = sf::Color(rand()%255, rand()%255, rand()%255);
		bodies.push_back(Body(radius, Vector(x, y), Vector(vx, vy), color));
	}
	system.setBodies(bodies);
	system.nullifySystemVelocity();
	return system;
}

bool valid(const NBodySystem& system, int width, int height)
{
	return !system.collisionOccured() && system.inArea(0, 0, width, height);
}

bool test(NBodySystem system, int width, int height)
{
	for(int i = 0; i < TEST_ITERATIONS; ++i)
	{
		system.tick(TIMEDELTA_TEST);
		if(!valid(system, width, height))
			return false;
	}
	return true;
}

NBodySystem generateSystem(int width, int height)
{
	for(int i = 0; ; i++)
	{
		NBodySystem system = initializeSystem(width, height);
		if(test(system, width, height) || i > MAX_TESTS)
			return system;
	}
}
