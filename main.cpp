#include <thread>
#include <SFML/Graphics.hpp>
#include "NBodySystem.hpp"
#include <iostream>

const int MICROSECONDS_PER_TICK = 300;

void initialize(NBodySystem& system)
{
    system.setBodies(std::vector<Body>{
		Body(5, Vector(400, 100), Vector(1.1, 0)),
		Body(10, Vector(500, 200), Vector(0, 0)),
		Body(3, Vector(600, 400), Vector(0.9, -0.5)),
	});
	system.nullifySystemVelocity();
}

void draw(const NBodySystem& system, sf::RenderWindow& window)
{
	for(Body body : system.getBodies())
	{
		sf::CircleShape circle(body.radius);
		circle.setPosition(body.position.x - body.radius, body.position.y - body.radius);
		window.draw(circle);
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
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "Orbit");
	window.setFramerateLimit(50);

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

        draw(system, window);

        // Update the window
        window.display();
    }
    run = false;
    tickThread.join();

    return EXIT_SUCCESS;
}
