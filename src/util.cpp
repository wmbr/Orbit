#include <SFML/System/Vector2.hpp>

#include "util.hpp"


sf::Color darken(sf::Color color)
{
	return sf::Color(color.r/2, color.g/2, color.b/2, 128);
}

sf::Color randomColor(double bodyRadius)
{
	sf::Color color;
	do{
		color = sf::Color(rand()%255, rand()%255, rand()%255);
	}while(bodyRadius * brightness(color) < 500); //avoid hard to see bodies
	return color;
}

float brightness(sf::Color color)
{
	return 0.3*color.r + 0.6*color.g + 0.1*color.b;
}

NBodySystem initializeSystem(sf::Vector2u size)
{
	NBodySystem system;
	std::vector<Body> bodies;
	for(int i = 0; i < NUM_BODIES; ++i)
	{
		double radius = std::rand() % 12 + 3;
		double x = std::rand() % (size.x/2) + (size.x/4.0);
		double y = std::rand() % (size.y/2) + (size.y/4.0);
		double vx = (std::rand() % 100) / 100.0;
		double vy = (std::rand() % 100) / 100.0;
		bodies.push_back(Body(radius, Vector(x, y), Vector(vx, vy), randomColor(radius)));
	}
	system.setBodies(bodies);
	system.nullifySystemVelocity();
	return system;
}



bool valid(const NBodySystem& system, sf::Vector2u size)
{
	return !system.simulationTerminated() && system.inArea(0, 0, size.x, size.y);
}

bool test(NBodySystem system, sf::Vector2u size)
{
	for(int i = 0; i < TEST_ITERATIONS; ++i)
	{
		system.tick(TIMEDELTA_TEST);
		if(!valid(system, size))
			return false;
	}
	return true;
}

NBodySystem generateSystem(sf::Vector2u size)
{
	for(int i = 0; ; i++)
	{
		NBodySystem system = initializeSystem(size);
		if(test(system, size) || i > MAX_TESTS)
			return system;
	}
}
