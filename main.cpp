#include <thread>
#include <SFML/Graphics.hpp>
#include "NBodySystem.hpp"
#include <iostream>

const int MICROSECONDS_PER_TICK = 300;

void initialize(NBodySystem& system)
{
    system.setBodies(std::vector<Body>{
		Body(5, Vector(400, 300), Vector(1.2, 0), sf::Color::Red),
		Body(10, Vector(500, 400), Vector(0, 0), sf::Color::Yellow),
		Body(3, Vector(600, 500), Vector(0.9, -0.5), sf::Color::Cyan),
	});
	system.nullifySystemVelocity();
}

void draw(const NBodySystem& system, sf::RenderWindow& window, sf::RenderTexture& trails)
{
	for(Body body : system.getBodies())
	{
		sf::CircleShape circle(body.radius);
		circle.setPosition(body.position.x - body.radius, body.position.y - body.radius);
		circle.setFillColor(body.color);
        trails.draw(circle);
	}
    sf::Sprite sprite(trails.getTexture());
    window.draw(sprite);
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
    // Create the main window
    sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(videoMode, "Orbit", sf::Style::Default, settings);
	window.setFramerateLimit(50);

	sf::RenderTexture trails;

	if(!trails.create(videoMode.width, videoMode.height) || !sf::Shader::isAvailable())
        return -1;

    trails.clear(sf::Color::Black);


    NBodySystem system;
    initialize(system);
    bool run = true;
    std::thread tickThread(tickLoop, std::ref(system), std::ref(run));
//    tickThread.detach();

	// Start the game loop
    while (window.isOpen())
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

    return EXIT_SUCCESS;
}
