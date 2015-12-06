#include "../include/ParticleSystem.h"

ParticleSystem::ParticleSystem(std::vector<glm::vec3> positions)
{
	p.resize(positions.size());
	v.resize(positions.size());
	F.resize(positions.size());
	g.resize(positions.size());
	initialCenterOfMass = glm::vec3(0,0,0);
	for (int i = 0; i < positions.size(); ++i)
	{
		p[i] = positions[i];
		v[i] = glm::vec3(0,0,0);
		F[i] = glm::vec3(0,0,0);
		initialCenterOfMass += positions[i];
	}
	initialCenterOfMass *= 1.0f / static_cast<float>(positions.size());
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

	glm::mat3 A_pq = glm::mat3(0.0f);
	float particleMass = 1.0f / p0.size();
	glm::vec3 centerOfMass = computeCOM();

	// Compute A_pq matrix
	for(unsigned int i = 0; i < p0.size(); i++)
		A_pq += particleMass * (p[i] - centerOfMass) * (p0[i] - initialCenterOfMass);

	glm::mat3 S = mat3Sqrt(glm::transpose(A_pq) * A_pq);
	// Compute rotation matrix
	glm::mat3 R = A_pq * glm::inverse(S);

	// Compute target positions
	for(unsigned int i = 0; i < p0.size(); i++)
		g[i] = R * (p0[i] - initialCenterOfMass) + centerOfMass;
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

glm::vec3 ParticleSystem::computeCOM()
{
	glm::vec3 com = glm::vec3(0,0,0);

	for(unsigned int i = 0; i < p.size(); i++)
		com += p[i];

	return 1.0f / static_cast<float>(p.size()) * com;

}

glm::mat3 ParticleSystem::mat3Sqrt(glm::mat3 m)
{
	for(unsigned int i = 0; i < 3; i++) {
		for(unsigned int j = 0; j < 3; j++) {
			m[i][j] = sqrt(m[i][j]);
		}
	}
	return m;
}