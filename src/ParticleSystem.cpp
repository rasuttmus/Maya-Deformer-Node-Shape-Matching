#include "../include/ParticleSystem.h"
#include <string>
#include "../ext/glm/gtx/transform.hpp"

std::string to_string(arma::fmat m)
{
	std::string output("[");
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			output += " " + std::to_string(m(i, j)) + " , ";
		}
		output += "\n";
	}
	output += " ]";

	return output;
}

std::string to_string(arma::fvec v)
{
	std::string output("[");
	for (int i = 0; i < 3; ++i)
	{
		output += " " + std::to_string(v(i)) + " , ";
		output += "\n";
	}
	output += " ]";

	return output;
}

std::string to_string(glm::vec3 v)
{
	std::string output("[");
	for (int i = 0; i < 3; ++i)
	{
		output += " " + std::to_string(v[i]) + " , ";
		output += "\n";
	}
	output += " ]";

	return output;
}

std::string to_string(glm::mat3 m)
{
	std::string output("[");
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			output += " " + std::to_string(m[i][j]) + " , ";
		}
		output += "\n";
	}
	output += " ]";

	return output;
}


ParticleSystem::ParticleSystem(std::vector<glm::vec3> positions)
{
	p0.resize(positions.size());
	p.resize(positions.size());
	v.resize(positions.size());
	F.resize(positions.size());
	g.resize(positions.size());
	initialCenterOfMass = glm::vec3(0,0,0);
	for (int i = 0; i < positions.size(); ++i)
	{
		p0[i] = positions[i];
		p[i] = positions[i];
		g[i] = positions[i];
		v[i] = glm::vec3(0,0,0);
		F[i] = glm::vec3(0,0,0);
		initialCenterOfMass += positions[i];
	}
	initialCenterOfMass *= 1.0f / static_cast<float>(positions.size());
	arma::mat A = arma::randu<arma::mat>(4,5);
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
		V
			<< V(0,0) << V(0,1) << -V(0,2) << arma::endr
			<< V(1,0) << V(1,1) << -V(1,2) << arma::endr
			<< V(2,0) << V(2,1) << -V(2,2) << arma::endr;
		R = V * U.t();
	}

	// Convert to glm matrix
	glm::mat3 R_glm;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			R_glm[i][j] = R(i,j);
	// R_glm = glm::mat3(glm::rotate(3.1415f / 8, glm::vec3(1.0f,0.0f,0.0f)));
	
	// Compute target positions
	for(unsigned int i = 0; i < p0.size(); i++)
		g[i] = R_glm * (p0[i] - initialCenterOfMass) + centerOfMass;

	output = "R : \n";
	output += to_string(R);
	MGlobal::displayInfo(output.c_str());
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

		// Add shape matching
		v[i] += 0.5f * (g[i] - p[i]) / dt;

		F[i] = glm::vec3(0,0,0);
	}
}

void ParticleSystem::updatePositions(float dt)
{
	//Euler integration
	for (int i = 0; i < p.size(); ++i)
	{
		p[i] += v[i] * dt;
		// Add shape matching (or not?)
		//p[i] += 1.0f * (g[i] - p[i]);
	}	
}

glm::vec3 ParticleSystem::computeCOM()
{
	glm::vec3 com = glm::vec3(0,0,0);

	for(unsigned int i = 0; i < p.size(); i++)
		com += p[i];

	return (1.0f / static_cast<float>(p.size())) * com;

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