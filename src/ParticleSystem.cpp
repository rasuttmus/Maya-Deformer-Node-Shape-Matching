#include "../include/ParticleSystem.h"

ParticleSystem::ParticleSystem(std::vector<glm::vec3> positions)
{
	p.resize(positions.size());
	v.resize(positions.size());
	F.resize(positions.size());

	for (int i = 0; i < positions.size(); ++i)
	{
		p[i] = positions[i];
		v[i] = glm::vec3(0,0,0);
		F[i] = glm::vec3(0,0,0);
	}
}

ParticleSystem::~ParticleSystem()
{
	
}

void ParticleSystem::stepPhysics(float dt)
{
	updateForces(dt);
	updateVelocities(dt);
	updatePositions(dt);
}

void ParticleSystem::matchShape(float dt)
{
	// 1. Find the rigid body transform R and t
	// 2. Transform the initial positions
}

glm::vec3 ParticleSystem::getPosition(int i)
{
	return p[i];
}

void ParticleSystem::updateForces(float dt)
{
	// Should set forces according to input and collisions etc
	for (int i = 0; i < F.size(); ++i)
	{
		// Add gravity
		F[i] = glm::vec3(0,-9.82,0);

		// Add collision impulse
		if (p[i].y <= 0)
		{
			glm::vec3 vDiff = v[i] - glm::vec3(0,0,0);
			float elasticity = 1;
			float mass = 1;
			glm::vec3 impulse = -(elasticity + 1) * vDiff * mass;
			
			F[i] += impulse / dt;
			p[i].y = 0.01;
		}
	}
}

void ParticleSystem::updateVelocities(float dt)
{
	// Euler integration
	for (int i = 0; i < v.size(); ++i)
	{
		float mass = 1;
		v[i] += F[i] / mass * dt;
		F[i] = glm::vec3(0,0,0);
	}
}

void ParticleSystem::updatePositions(float dt)
{
	// Euler integration
	for (int i = 0; i < p.size(); ++i)
	{
		p[i] += v[i] * dt;
	}	
}