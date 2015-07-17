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
	sf::Vector2u size = window.getSize();

	sf::RenderTexture trails;

	if(!trails.create(videoMode.width, videoMode.height))
		throw std::runtime_error("Could not create RenderTexture");

	GaussianBlur blur;

	system = generateSystem(size);

	while (true)
	{
		trails.clear(sf::Color::Black);

		runTickThread = true;
		tickThread = std::thread(tickLoop, std::ref(system), std::ref(runTickThread));

		while(valid(system, size))
		{
			handleEvents(window);

			window.clear();
			draw(system, window, trails, blur);

			// Update the window
			window.display();
		}

		if(!valid(system, size))
		{
			sf::Clock clock;

			//terminate tickThread:
			runTickThread = false;

			//generate new system:
			auto generationFuture = std::async(std::launch::async, generateSystem, size);

			tickThread.join();

			sf::RectangleShape fadeRect = sf::RectangleShape(sf::Vector2f(size));
			while(clock.getElapsedTime().asMilliseconds() < 3000)
			{
				handleEvents(window);

				window.clear();
				draw(system, window, trails, blur);

				sf::Color fadeBlack = sf::Color(0, 0, 0, std::min(255, clock.getElapsedTime().asMilliseconds()/10));
				fadeRect.setFillColor(fadeBlack);
				window.draw(fadeRect);

				window.display();
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
