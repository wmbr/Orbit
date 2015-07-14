#include <thread>
#include <iostream>
#include <cstdlib>
#include <future>

#include <SFML/Graphics.hpp>

#include "NBodySystem.hpp"

const int MICROSECONDS_PER_TICK = 200;
const int NUM_BODIES = 3;
const double TIMEDELTA = 0.05;
const double TIMEDELTA_TEST = 0.5;
const int TEST_ITERATIONS = 10000;
const int MAX_TESTS = 1000;

sf::Color darken(sf::Color color)
{
	return sf::Color(color.r/2, color.g/2, color.b/2, 32);
}

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




void draw(const NBodySystem& system, sf::RenderWindow& window, sf::RenderTexture& trails)
{
	sf::Sprite sprite(trails.getTexture());
	sprite.setScale(1, -1);
	sprite.setPosition(0, trails.getSize().y);
	window.draw(sprite);
	for(Body body : system.getBodies())
	{
		sf::CircleShape circle(body.radius);
		circle.setOrigin(body.radius, body.radius);
		circle.setPosition(body.position.x, body.position.y);
		circle.setFillColor(body.color);
		window.draw(circle);
		circle.scale(0.5, 0.5);
		circle.setFillColor(darken(body.color));
		trails.draw(circle);
	}
}

void tickLoop(NBodySystem& system, const bool& run)
{
	while(run)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(MICROSECONDS_PER_TICK));
		system.tick(TIMEDELTA);
		if(system.collisionOccured())
			return;
	}
}



class ProgramTermination
{};

void handleEvents(sf::Window& window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			throw ProgramTermination();
		}
	}
}



int main()
{
	std::srand(std::time(nullptr));

	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(videoMode, "Orbit", sf::Style::Default, settings);
	window.setFramerateLimit(120);

	sf::RenderTexture trails;

	if(!trails.create(videoMode.width, videoMode.height) || !sf::Shader::isAvailable())
		return -1;

	NBodySystem system = generateSystem(videoMode.width, videoMode.height);
	std::thread tickThread;
	bool runTickThread;
	try
	{
		while (true)
		{
			trails.clear(sf::Color::Black);

			runTickThread = true;
			tickThread = std::thread(tickLoop, std::ref(system), std::ref(runTickThread));

			while(valid(system, videoMode.width, videoMode.height))
			{
				handleEvents(window);

				window.clear(sf::Color::Black);
				draw(system, window, trails);

				// Update the window
				window.display();
			}

			if(!valid(system, videoMode.width, videoMode.height))
			{
				sf::Clock clock;

				draw(system, window, trails);
				window.display();

				//generate new system:
				auto generationFuture = std::async(std::launch::async, generateSystem, videoMode.width, videoMode.height);

				//terminate tickThread:
				runTickThread = false;
				tickThread.join();

				while(clock.getElapsedTime().asMilliseconds() < 3000)
				{
					handleEvents(window);
					sf::sleep(sf::milliseconds(10));
				}
				system = generationFuture.get();
			}
		}
	}
	catch(ProgramTermination)
	{
			//terminate tickThread:
		runTickThread = false;
		if(tickThread.joinable())
			tickThread.join();
		return EXIT_SUCCESS;
	}
}
