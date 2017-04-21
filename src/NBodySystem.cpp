#include <cmath>

#include "NBodySystem.hpp"
#include "util.hpp"

double abs(Vector v)
{
	return std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
}

Body::Body(double radius, Vector pos, Vector velocity, sf::Color color)
	: radius(radius), position(pos), velocity(velocity), color(color)
{}

double Body::mass() const
{
	return this->radius * this->radius;
}

bool Body::inArea(double x1, double y1, double x2, double y2) const
{
	return x1 < this->position.x+this->radius && x2 > this->position.x-this->radius && y1 < this->position.y+this->radius && y2 > this->position.y-this->radius;
}

void NBodySystem::tick(double timedelta)
{
	if(this->stopSimulation)
		return;
	auto writelock = this->announceWrite();
	auto newstate = this->bodies;
	for(std::size_t i = 0; i < newstate.size(); ++i)
	{
		newstate[i].position += timedelta * this->bodies[i].velocity;
		for(std::size_t j = 0; j < this->bodies.size(); ++j)
		{
			if(i == j)
				continue;
			auto m = this->bodies[j].mass();
			Vector d = this->bodies[j].position - this->bodies[i].position;
			newstate[i].velocity += timedelta * GRAVITY_CONSTANT * m / std::pow(abs(d), 3) * d;
		}
	}
	this->handleCollisions(newstate);
	auto excllock = this->lockExclusive();
	this->bodies = newstate;
}



NBodySystem::NBodySystem(const NBodySystem& system)
	: bodies(system.bodies), collisionBehaviour(system.collisionBehaviour), stopSimulation(system.stopSimulation)
{
}

NBodySystem& NBodySystem::operator=(const NBodySystem& system)
{
	auto writelock = this->announceWrite();
	auto excllock = this->lockExclusive();
	this->bodies = system.bodies;
	this->collisionBehaviour = system.collisionBehaviour;
	this->stopSimulation = system.stopSimulation;
	return *this;
}


std::vector<Body> NBodySystem::getBodies() const
{
	auto lock = this->lockShared();
	return this->bodies;
}

void NBodySystem::setBodies(const std::vector<Body>& bodies)
{
	auto writelock = this->announceWrite();
	auto excllock = this->lockExclusive();
	this->bodies = bodies;
	this->stopSimulation = false;
}

void NBodySystem::handleCollisions(std::vector<Body>& bodies)
{
	if(this->collisionBehaviour == CollisionBehaviour::IGNORE)
		return;
	for(std::size_t i = 0; i < bodies.size(); ++i)
	{
		for(std::size_t j = i+1; j < bodies.size() && i < bodies.size(); ++j)
		{
			if(abs(bodies[i].position - bodies[j].position) < (bodies[i].radius + bodies[j].radius))
			{
				switch(this->collisionBehaviour)
				{
					case CollisionBehaviour::TERMINATE:
						this->stopSimulation = true;
						return;
					case CollisionBehaviour::MERGE:
						bodies.push_back(this->mergeBodies(bodies[i], bodies[j]));
						//fallthrough
					case CollisionBehaviour::DELETE:
						bodies.erase(bodies.begin()+j);
						bodies.erase(bodies.begin()+i);
						if(bodies.size() <= 1)
						{
							this->stopSimulation = true;
							return;
						}
						j = i; //bodies[i] is now the next body, so we need to restart at j=i+1
						break;
					case CollisionBehaviour::IGNORE:
						break;
				}
			}
		}
	}
}

Body NBodySystem::mergeBodies(const Body& b1, const Body& b2) const
{
	double radius = std::sqrt(b1.mass() + b2.mass());
	Vector pos = (b1.mass() * b1.position + b2.mass() * b2.position) / (b1.mass() + b2.mass());
	Vector v = (b1.mass() * b1.velocity + b2.mass() * b2.velocity) / (b1.mass() + b2.mass());
	sf::Color color = b1.mass() > b2.mass() ? b1.color : b2.color;
	return Body(radius, pos, v, color);
}

bool NBodySystem::simulationTerminated() const
{
	return this->stopSimulation;
}

Vector NBodySystem::getCenterOfMass() const
{
	auto lock = this->lockShared();
	Vector com = Vector(0, 0);
	for(Body body : this->bodies)
	{
		com += body.mass() * body.position;
	}
	return com;
}

Vector NBodySystem::getSystemVelocity() const
{
	auto lock = this->lockShared();
	Vector v = Vector(0, 0);
	double m = 0;
	for(Body body : this->bodies)
	{
		v += body.mass() * body.velocity;
		m += body.mass();
	}
	return v/m;
}

void NBodySystem::nullifySystemVelocity()
{
	auto writelock = this->announceWrite();
	Vector v = this->getSystemVelocity();
	auto excllock = this->lockExclusive();
	for(Body& body : this->bodies)
	{
		body.velocity -= v;
	}
}

bool NBodySystem::inArea(double x1, double y1, double x2, double y2) const
{
	auto lock = this->lockShared();
	for(Body body : this->bodies)
	{
		if(body.inArea(x1, y1, x2, y2))
			return true;
	}
	return false;
}


std::shared_lock<std::shared_timed_mutex> NBodySystem::lockShared() const
{
	return std::shared_lock<std::shared_timed_mutex>(this->readMutex);
}

std::unique_lock<std::shared_timed_mutex> NBodySystem::lockExclusive()
{
	return std::unique_lock<std::shared_timed_mutex>(this->readMutex);
}

std::unique_lock<std::mutex> NBodySystem::announceWrite()
{
	return std::unique_lock<std::mutex>(this->writeMutex);
}
