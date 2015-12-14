#include "../include/ParticleSystem.h"

ParticleSystem::ParticleSystem(std::vector<glm::vec3> positions)
{
	p0.resize(positions.size());
	p.resize(positions.size());
	v.resize(positions.size());
	v_m_prev.resize(positions.size());
	F.resize(positions.size());
	g.resize(positions.size());
	initialCenterOfMass = glm::vec3(0,0,0);
	for (int i = 0; i < positions.size(); ++i)
	{
		p0[i] = positions[i];
		p[i] = positions[i];
		g[i] = positions[i];
		v[i] = glm::vec3(0,0,0);
		v_m_prev[i] = glm::vec3(0,0,0);
		F[i] = glm::vec3(0,0,0);
		initialCenterOfMass += positions[i];
	}
	initialCenterOfMass *= 1.0f / static_cast<float>(positions.size());
	arma::mat A = arma::randu<arma::mat>(4,5);
}

ParticleSystem::~ParticleSystem()
{
	
}

void ParticleSystem::stepPhysics(float dt, PhysicsArguments pArg)
{
	updateForces(dt, pArg);
	updateVelocities(dt, pArg);
	updatePositions(dt);
}

void ParticleSystem::matchShape(float dt, PhysicsArguments pArg)
{
	std::string output("");

	// Original positions and target positions locally
	arma::fmat X; // Original positions 
	arma::fmat Y; // Target positions

	arma::fmat S; // Covariance matrix
	arma::fmat U; // Rotation matrix
	arma::fvec s; // Eigen values
	arma::fmat V; // Rotation matrix
	arma::fmat R; // Final rotation matrix

	glm::vec3 centerOfMass;

	// Allocate
	X = arma::fmat(3, p.size());
	Y = arma::fmat(3, p.size());
	
	// Set X and Y matrices
	centerOfMass = computeCOM();
	for (int i = 0; i < p.size(); ++i)
	{
		X(0,i) = p[i].x - centerOfMass.x;
		X(1,i) = p[i].y - centerOfMass.y;
		X(2,i) = p[i].z - centerOfMass.z;

		Y(0,i) = p0[i].x - initialCenterOfMass.x;
		Y(1,i) = p0[i].y - initialCenterOfMass.y;
		Y(2,i) = p0[i].z - initialCenterOfMass.z;
	}

	// Compute the covariance matrix, svd and rotation
	S = X * Y.t();
	arma::svd(U,s,V,S);
	R = V * U.t();
	
	// If rotation is a reflection, reflect back
	if (det(R) < 0)
	{
		R(0,2) = -R(0,2);
		R(1,2) = -R(1,2);
		R(2,2) = -R(2,2);
	}

	// Convert to glm matrix
	glm::mat3 R_glm = to_glm(R);
	
	// Compute target positions
	for(unsigned int i = 0; i < p0.size(); i++)
		g[i] = R_glm * (p0[i] - initialCenterOfMass) + centerOfMass;

	// Add shape matching
	for (int i = 0; i < p.size(); ++i)
	{
		v[i] += pArg.stiffness * (g[i] - p[i]) / dt - v_m_prev[i];		
		p[i] += v[i] * dt;
	}	
}

glm::vec3 ParticleSystem::getPosition(int i)
{
	return p[i];
}

void ParticleSystem::updateForces(float dt, PhysicsArguments pArg)
{
	// Should set forces according to input and collisions etc
	for (int i = 0; i < F.size(); ++i)
	{
		// Add gravity
		F[i] = pArg.gravity;

		// Add collision impulse
		if (p[i].y <= 0)
		{
			glm::vec3 vDiff = (v[i]) - glm::vec3(0,0,0);
			glm::vec3 impulse = -(pArg.elasticity + 1) * vDiff * pArg.mass;
			
			F[i] += impulse / dt;
			p[i].y = 0.01;
		}
	}
}

void ParticleSystem::updateVelocities(float dt, PhysicsArguments pArg)
{
	// Euler integration
	for (int i = 0; i < v.size(); ++i)
	{
		v[i] += F[i] / pArg.mass * dt;
		F[i] = glm::vec3(0,0,0);
	}
}

void ParticleSystem::updatePositions(float dt)
{
	//Euler integration
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

	return (1.0f / static_cast<float>(p.size())) * com;
}
