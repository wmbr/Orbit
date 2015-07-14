#ifndef NBODYSYSTEM_HPP
#define NBODYSYSTEM_HPP
#include <vector>
#include <shared_mutex>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

typedef sf::Vector2<double> Vector;

class Body
{
	public:
		double radius;
		Vector position;
		Vector velocity;
		sf::Color color;
		Body(double radius, Vector pos, Vector velocity, sf::Color color);
		double mass() const;
		bool inArea(double x1, double y1, double x2, double y2) const;
};

class NBodySystem
{
	private:
		std::vector<Body> bodies;
		mutable std::shared_timed_mutex readMutex;
		std::mutex writeMutex;
		bool collision = false;
		bool checkCollision();

		/**
		 * Aquire before reading from this->bodies.
		 */
		std::shared_lock<std::shared_timed_mutex> lockShared() const;

		/**
		 * Aquire before writing to this->bodies.
		 */
		std::unique_lock<std::shared_timed_mutex> lockExclusive();

		/**
		 * Aquire to prevent further writes to this->bodies.
		 */
		std::unique_lock<std::mutex> announceWrite();

	public:
		std::vector<Body> getBodies() const;
		void setBodies(const std::vector<Body>& bodies);
		void tick(double timedelta);
		bool collisionOccured() const;
		NBodySystem() = default;
		NBodySystem(const NBodySystem& system);
		Vector getCenterOfMass() const;
		Vector getSystemVelocity() const;
		void nullifySystemVelocity();
		bool inArea(double x1, double y1, double x2, double y2) const;
};

#endif // NBODYSYSTEM_HPP
