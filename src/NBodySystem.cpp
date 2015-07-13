#include "NBodySystem.hpp"
#include <cmath>

double abs(Vector v)
{
	return std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
}

double Body::mass() const
{
	return this->radius * this->radius;
}

void NBodySystem::tick(double timedelta)
{
	auto writelock = this->announceWrite();
	auto readlock = this->lockShared();
	if(this->collisionOccured())
		return;
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
			newstate[i].velocity += timedelta * m / std::pow(abs(d), 3) * d;
		}
	}
	readlock.unlock();
	auto excllock = this->lockExclusive();
	this->bodies = newstate;
	excllock.unlock();
    this->checkCollision();
}

NBodySystem::NBodySystem()
	: bodies()
{}

Body::Body(double radius, Vector pos, Vector v)
	: radius(radius), position(pos), velocity(v)
{}

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
	this->collision = false;
}

bool NBodySystem::checkCollision()
{
	auto lock = this->lockShared();
	for(std::size_t i = 0; i < this->bodies.size(); ++i)
	{
		for(std::size_t j = 0; j < this->bodies.size(); ++j)
		{
			if(i == j)
				continue;
			if(abs(this->bodies[i].position - this->bodies[j].position) < (this->bodies[i].radius + this->bodies[j].radius))
			{
				this->collision = true;
				return true;
			}
		}
	}
	return false;
}

bool NBodySystem::collisionOccured() const
{
	return this->collision;
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
