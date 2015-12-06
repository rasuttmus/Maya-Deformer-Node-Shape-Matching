#include "../include/ParticleSystem.h"
#include <string>

std::string armaMatrixToString(arma::fmat m)
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
	// 1. Find the rigid body transform R and t
	// 2. Transform the initial positions

	glm::mat3 A_pq_glm = glm::mat3(0.0f);
	float particleMass = 1.0f;
	glm::vec3 centerOfMass = computeCOM();

	// Compute A_pq_glm matrix
	for(unsigned int i = 0; i < p0.size(); i++)
		A_pq_glm += particleMass * (p[i] - centerOfMass) * (p0[i] - initialCenterOfMass);

	arma::fmat A_pq((float*)&A_pq_glm[0], 3, 3);

	std::string output("A_pq : \n");
	output += armaMatrixToString(A_pq);
	MGlobal::displayInfo(output.c_str());

	// Compute the sqare root of A_pq.t() * A_pq = S
	arma::fvec eigval;
	arma::fmat V;
	arma::eig_sym(eigval, V, A_pq.t() * A_pq);
	arma::fmat DSqrt(3,3);
	DSqrt.eye(3,3);

	DSqrt(0,0) = eigval(0);
	DSqrt(1,1) = eigval(1);
	DSqrt(2,2) = eigval(2);

	output = "DSqrt (before sqrt) : \n";
	output += armaMatrixToString(DSqrt);
	MGlobal::displayInfo(output.c_str());

	DSqrt(0,0) = sqrt(std::abs(DSqrt(0,0)));
	DSqrt(1,1) = sqrt(std::abs(DSqrt(1,1)));
	DSqrt(2,2) = sqrt(std::abs(DSqrt(2,2)));

	output = "DSqrt (after sqrt) : \n";
	output += armaMatrixToString(DSqrt);
	MGlobal::displayInfo(output.c_str());


	output = "V : \n";
	output += armaMatrixToString(V);
	MGlobal::displayInfo(output.c_str());


	output = "A_pq.t() * A_pq : \n";
	output += armaMatrixToString(A_pq.t() * A_pq);
	MGlobal::displayInfo(output.c_str());


	output = "DSqrt : \n";
	output += armaMatrixToString(DSqrt);
	MGlobal::displayInfo(output.c_str());

	arma::fmat S = V * DSqrt * V.t();


	output = "S : \n";
	output += armaMatrixToString(S);
	MGlobal::displayInfo(output.c_str());

	// Rotation matrix
	arma::fmat R = A_pq * S.i();
	//R.eye();

	output = "R : \n";
	output += armaMatrixToString(R);
	MGlobal::displayInfo(output.c_str());


	glm::mat3 R_glm;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			R_glm[i][j] = R(i,j);

	// Compute target positions
	for(unsigned int i = 0; i < p0.size(); i++)
		g[i] = R_glm * (p0[i] - initialCenterOfMass) + centerOfMass;

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
		//v[i] += 0.1f * (g[i] - p[i]) / dt;

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