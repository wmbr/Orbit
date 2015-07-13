#include <thread>
#include <SFML/Graphics.hpp>
#include "NBodySystem.hpp"
#include <iostream>
#include <cstdlib>

const int MICROSECONDS_PER_TICK = 300;
const int NUM_BODIES = 3;

void initialize(NBodySystem& system, int width, int height)
{
	std::vector<Body> bodies;
	for(int i = 0; i < NUM_BODIES; ++i)
	{
		double radius = std::rand() % 20 + 1;
		double x = std::rand() % (width/2) + (width/4.0);
		double y = std::rand() % (height/2) + (height/4.0);
		double vx = (std::rand() % 100) / 100.0;
		double vy = (std::rand() % 100) / 100.0;
		sf::Color color = sf::Color(rand()%255, rand()%255, rand()%255);
		bodies.push_back(Body(radius, Vector(x, y), Vector(vx, vy), color));
	}
	system.setBodies(bodies);
	system.nullifySystemVelocity();
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
		trails.draw(circle);
	}
}

void tickLoop(NBodySystem& system, const bool& run)
{
	while(run)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(MICROSECONDS_PER_TICK));
		system.tick(0.1);
		if(system.collisionOccured())
			return;
	}
}

int main()
{
	std::srand(std::time(nullptr));
	// Create the main window
	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(videoMode, "Orbit", sf::Style::Default, settings);
	window.setFramerateLimit(120);

	sf::RenderTexture trails;

	if(!trails.create(videoMode.width, videoMode.height) || !sf::Shader::isAvailable())
		return -1;


	while (window.isOpen())
	{
		trails.clear(sf::Color::Black);

		NBodySystem system;
		initialize(system, videoMode.width, videoMode.height);
		bool run = true;
		std::thread tickThread(tickLoop, std::ref(system), std::ref(run));

		while(!system.collisionOccured() && window.isOpen())
		{
			// Process events
			sf::Event event;
			while (window.pollEvent(event))
			{
				// Close window : exit
				if (event.type == sf::Event::Closed)
					window.close();
			}

			// Clear screen
			window.clear(sf::Color::Black);

			draw(system, window, trails);

			// Update the window
			window.display();
		}
		run = false;
		tickThread.join();

		if(system.collisionOccured())
		{
			draw(system, window, trails);

			// Update the window
			window.display();
			sf::sleep(sf::seconds(3));
		}
	}

	return EXIT_SUCCESS;
}
