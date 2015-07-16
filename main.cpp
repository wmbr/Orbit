#include <thread>
#include <iostream>
#include <cstdlib>
#include <future>

#include <SFML/Graphics.hpp>

#include "NBodySystem.hpp"
#include "GaussianBlur.hpp"
#include "util.hpp"


class ProgramTermination
{};

sf::Color darken(sf::Color color)
{
	return sf::Color(color.r/2, color.g/2, color.b/2, 128);
}

void draw(const NBodySystem& system, sf::RenderWindow& window, sf::RenderTexture& trails, GaussianBlur& blur)
{
	blur.apply(window, trails.getTexture());

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
	std::thread tickThread;
	bool runTickThread;
	NBodySystem system;

	try	{

	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(videoMode, "Orbit", sf::Style::Default, settings);
	window.setFramerateLimit(120);

	sf::RenderTexture trails;

	if(!trails.create(videoMode.width, videoMode.height))
		throw std::runtime_error("Could not create RenderTexture");

	GaussianBlur blur;

	system = generateSystem(videoMode.width, videoMode.height);

	while (true)
	{
		trails.clear(sf::Color::Black);

		runTickThread = true;
		tickThread = std::thread(tickLoop, std::ref(system), std::ref(runTickThread));

		while(valid(system, videoMode.width, videoMode.height))
		{
			handleEvents(window);

			window.clear(sf::Color::Black);
			draw(system, window, trails, blur);

			// Update the window
			window.display();
		}

		if(!valid(system, videoMode.width, videoMode.height))
		{
			sf::Clock clock;

			//terminate tickThread:
			runTickThread = false;

			draw(system, window, trails, blur);
			window.display();

			tickThread.join();

			//generate new system:
			auto generationFuture = std::async(std::launch::async, generateSystem, videoMode.width, videoMode.height);

			while(clock.getElapsedTime().asMilliseconds() < 3000)
			{
				handleEvents(window);
				sf::sleep(sf::milliseconds(10));
			}
			system = generationFuture.get();
		}
	}
	} catch(ProgramTermination)	{
		//terminate tickThread:
		runTickThread = false;
		if(tickThread.joinable())
			tickThread.join();
		return EXIT_SUCCESS;
	} catch(std::runtime_error e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

}
