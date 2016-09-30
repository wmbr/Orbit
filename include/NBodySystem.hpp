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
	public:
		/**
		 * How to handle body collsions.
		 */
		enum class CollisionBehaviour
		{
			TERMINATE, ///< terminate simulation
			MERGE,     ///< merge colliding bodies
			DELETE,    ///< delete colliding bodies
			IGNORE     ///< ignore collisions (might cause rounding errors)
		};
		std::vector<Body> getBodies() const;
		void setBodies(const std::vector<Body>& bodies);
		void tick(double timedelta);
		bool simulationTerminated() const;
		NBodySystem() = default;
		NBodySystem(const NBodySystem& system);
		NBodySystem& operator=(const NBodySystem&);
		Vector getCenterOfMass() const;
		Vector getSystemVelocity() const;
		void nullifySystemVelocity();
		bool inArea(double x1, double y1, double x2, double y2) const;

	private:
		std::vector<Body> bodies;
		mutable std::shared_timed_mutex readMutex;
		std::mutex writeMutex;
		CollisionBehaviour collisionBehaviour = CollisionBehaviour::TERMINATE;
		bool stopSimulation = false;
		void handleCollisions(std::vector<Body>& bodies);
		Body mergeBodies(const Body& b1, const Body& b2) const;

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
};

#endif // NBODYSYSTEM_HPP
