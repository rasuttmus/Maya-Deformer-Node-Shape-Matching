#include "../include/ParticleSystem.h"
ParticleSystem::ParticleSystem(std::vector<glm::vec3> positions)
{
	// Reset all attributes
	p0.resize(positions.size());
	p.resize(positions.size());
	v.resize(positions.size());
	F.resize(positions.size());
	g.resize(positions.size());
	for (int i = 0; i < positions.size(); ++i)
	{
		p0[i] = positions[i];
		p[i] = positions[i];
		g[i] = positions[i];
		v[i] = glm::vec3(0,0,0);
		F[i] = glm::vec3(0,0,0);
	}
	initialCenterOfMass = computeCOM();

	// Calculate initial covarianve matrix Aqq
	// Allocate
	arma::fmat X = arma::fmat(3, p.size());
	
	// Set X and Y matrices (we assume that all particles have the same mass)
	for (int i = 0; i < p.size(); ++i)
	{
		X(0,i) = p0[i].x - initialCenterOfMass.x;
		X(1,i) = p0[i].y - initialCenterOfMass.y;
		X(2,i) = p0[i].z - initialCenterOfMass.z;
	}
	// Compute the inverse covariance matrix
	Aqq = (X * X.t()).i();
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

	arma::fmat A; // Linear transformation matrix
	arma::fmat I; // Linear transformation matrix
	arma::fmat Apq; // Covariance matrix
	arma::fmat U; // Rotation matrix
	arma::fvec s; // Eigen values
	arma::fmat V; // Rotation matrix
	arma::fmat R; // Final rotation matrix
	arma::fmat AR; // Final linear transformation combined with rotation

	glm::vec3 centerOfMass;
	glm::vec3 initialCenterOfMass2;

	// Allocate
	X = arma::fmat(3, p.size());
	Y = arma::fmat(3, p.size());
	I = arma::fmat(3,3);
	I.eye();

	// Set X and Y matrices (we assume that all particles have the same mass)
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

	// Compute matrices
	Apq = X * Y.t();
	arma::svd(U,s,V,Apq);
	R = V * U.t();
	//A = Apq * Aqq;
	
	// If rotation has a reflection, reflect back
	if (det(R) < 0)
	{
		R(0,2) = -R(0,2);
		R(1,2) = -R(1,2);
		R(2,2) = -R(2,2);
	}

	// Make sure volume is conserved for linear transformation
	//A /= pow(arma::det(A), 1/3.0f);

	// Linear combination of rotation and linear transformation matrix
	//AR = (pArg.deformation * R * A * R + (1 - pArg.deformation) * R); // The Stefan Löfvén operation

	// Convert to glm matrix
	glm::mat3 R_glm = to_glm(R);
	//glm::mat3 R_glm = to_glm(R);
	
	// Compute target positions
	for(unsigned int i = 0; i < p0.size(); i++)
		g[i] = R_glm * (p0[i] - initialCenterOfMass) + centerOfMass;




	// Allocate
	X = arma::fmat(3, p.size());
	Y = arma::fmat(9, p.size());
	I = arma::fmat(3, 9);
	
	I 	<< 1 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << arma::endr
		<< 0 << 1 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << arma::endr
		<< 0 << 0 << 1 << 0 << 0 << 0 << 0 << 0 << 0 << arma::endr;

	// Set X and Y matrices (we assume that all particles have the same mass)
	centerOfMass = computeCOM();







	initialCenterOfMass2 = glm::vec3(0,0,0);
	for(unsigned int i = 0; i < p.size(); i++)
		initialCenterOfMass2 += g[i];
	initialCenterOfMass2 = 1.0f / static_cast<float>(p.size()) * initialCenterOfMass2;

	arma::fmat X2 = arma::fmat(9, p.size());
	// Set X and Y matrices (we assume that all particles have the same mass)
	for (int i = 0; i < p.size(); ++i)
	{
		X2(0,i) = g[i].x - initialCenterOfMass2.x;
		X2(1,i) = g[i].y - initialCenterOfMass2.y;
		X2(2,i) = g[i].z - initialCenterOfMass2.z;
		X2(3,i) = (g[i].x - initialCenterOfMass2.x) * (g[i].x - initialCenterOfMass2.x);
		X2(4,i) = (g[i].y - initialCenterOfMass2.y) * (g[i].y - initialCenterOfMass2.y);
		X2(5,i) = (g[i].z - initialCenterOfMass2.z) * (g[i].z - initialCenterOfMass2.z);
		X2(6,i) = (g[i].x - initialCenterOfMass2.x) * (g[i].y - initialCenterOfMass2.y);
		X2(7,i) = (g[i].y - initialCenterOfMass2.y) * (g[i].z - initialCenterOfMass2.z);
		X2(8,i) = (g[i].z - initialCenterOfMass2.z) * (g[i].x - initialCenterOfMass2.x);
	}
	// Compute the inverse covariance matrix
	arma::fmat Aqq2 = (X2 * X2.t()).i();

	for (int i = 0; i < p.size(); ++i)
	{
		X(0,i) = p[i].x - centerOfMass.x;
		X(1,i) = p[i].y - centerOfMass.y;
		X(2,i) = p[i].z - centerOfMass.z;
		
		Y(0,i) = g[i].x - initialCenterOfMass2.x;
		Y(1,i) = g[i].y - initialCenterOfMass2.y;
		Y(2,i) = g[i].z - initialCenterOfMass2.z;
		Y(3,i) = (g[i].x - initialCenterOfMass2.x) * (g[i].x - initialCenterOfMass2.x);
		Y(4,i) = (g[i].y - initialCenterOfMass2.y) * (g[i].y - initialCenterOfMass2.y);
		Y(5,i) = (g[i].z - initialCenterOfMass2.z) * (g[i].z - initialCenterOfMass2.z);
		Y(6,i) = (g[i].x - initialCenterOfMass2.x) * (g[i].y - initialCenterOfMass2.y);
		Y(7,i) = (g[i].y - initialCenterOfMass2.y) * (g[i].z - initialCenterOfMass2.z);
		Y(8,i) = (g[i].z - initialCenterOfMass2.z) * (g[i].x - initialCenterOfMass2.x);
	}


/*
	// Set X and Y matrices (we assume that all particles have the same mass)
	initialCenterOfMass2 = glm::vec3(0,0,0);
	for(unsigned int i = 0; i < p.size(); i++)
		initialCenterOfMass2 += g[i];
	initialCenterOfMass2 = 1.0f / static_cast<float>(p.size()) * initialCenterOfMass2;

	arma::fmat X2 = arma::fmat(3, p.size());
	// Set X and Y matrices (we assume that all particles have the same mass)
	for (int i = 0; i < p.size(); ++i)
	{
		X2(0,i) = g[i].x - initialCenterOfMass2.x;
		X2(1,i) = g[i].y - initialCenterOfMass2.y;
		X2(2,i) = g[i].z - initialCenterOfMass2.z;


	}
	// Compute the inverse covariance matrix
	arma::fmat Aqq2 = (X2 * X2.t()).i();


	for (int i = 0; i < p.size(); ++i)
	{
		X(0,i) = p[i].x - centerOfMass.x;
		X(1,i) = p[i].y - centerOfMass.y;
		X(2,i) = p[i].z - centerOfMass.z;

		Y(0,i) = g[i].x - initialCenterOfMass2.x;
		Y(1,i) = g[i].y - initialCenterOfMass2.y;
		Y(2,i) = g[i].z - initialCenterOfMass2.z;
	}
	*/
	// Compute matrices
	Apq = X * Y.t();
	//arma::svd(U,s,V,Apq);
	//R = V * U.t();
	A = Apq * Aqq2;

	// If rotation has a reflection, reflect back
	//if (det(R) < 0)
	//{
	//	R(0,2) = -R(0,2);
	//	R(1,2) = -R(1,2);
	//	R(2,2) = -R(2,2);
	//}

	// Make sure volume is conserved for linear transformation
	//float scaleFactor = pow(arma::det(A) > 0.1 ? arma::det(A) : 0.1, 1/3.0f);
	//if (scaleFactor > 0.1)
	//	A /= scaleFactor;// pow(arma::det(A), 1/3.0f);


	// Linear combination of rotation and linear transformation matrix
	//float beta = 0.5;
	AR = (pArg.deformation * A + (1 - pArg.deformation) * I);

	arma::fmat AR_A = arma::fmat(3,3);
	arma::fmat AR_Q = arma::fmat(3,3);
	arma::fmat AR_M = arma::fmat(3,3);

	AR_A <<	AR(0,0) << AR(0,1) << AR(0,2) << arma::endr <<
			AR(1,0) << AR(1,1) << AR(1,2) << arma::endr <<
			AR(2,0) << AR(2,1) << AR(2,2) << arma::endr;

	AR_Q << AR(0,0 + 3) << AR(0,1 + 3) << AR(0,2 + 3) << arma::endr <<
			AR(1,0 + 3) << AR(1,1 + 3) << AR(1,2 + 3) << arma::endr <<
			AR(2,0 + 3) << AR(2,1 + 3) << AR(2,2 + 3) << arma::endr;

	AR_M << AR(0,0 + 6) << AR(0,1 + 6) << AR(0,2 + 6) << arma::endr <<
			AR(1,0 + 6) << AR(1,1 + 6) << AR(1,2 + 6) << arma::endr <<
			AR(2,0 + 6) << AR(2,1 + 6) << AR(2,2 + 6) << arma::endr;

	// Convert to glm matrix
	glm::mat3 AR_A_glm = to_glm(AR_A);
	glm::mat3 AR_Q_glm = to_glm(AR_Q);
	glm::mat3 AR_M_glm = to_glm(AR_M);
	//glm::mat3 R_glm = to_glm(R);
	
	// Compute target positions
	for(unsigned int i = 0; i < p0.size(); i++)
	{
		g[i] =
			AR_A_glm * (g[i] - initialCenterOfMass2) +//glm::vec3((g[i] - initialCenterOfMass2).x, (g[i] - initialCenterOfMass2).y, (g[i] - initialCenterOfMass2).z) +
			AR_Q_glm * glm::vec3((g[i] - initialCenterOfMass2).x * (g[i] - initialCenterOfMass2).x, (g[i] - initialCenterOfMass2).y * (g[i] - initialCenterOfMass2).y, (g[i] - initialCenterOfMass2).z * (g[i] - initialCenterOfMass2).z) +
			AR_M_glm * glm::vec3((g[i] - initialCenterOfMass2).x * (g[i] - initialCenterOfMass2).y, (g[i] - initialCenterOfMass2).y * (g[i] - initialCenterOfMass2).z, (g[i] - initialCenterOfMass2).z * (g[i] - initialCenterOfMass2).x) +
		 	centerOfMass;
	}

	// Add shape matching
	for (int i = 0; i < p.size(); ++i)
	{
		v[i] += pArg.flappyness * pArg.stiffness * (g[i] - p[i]) / dt;
		p[i] += pArg.stiffness * (g[i] - p[i]);
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
		F[i] = pArg.gravity * pArg.mass;

		// Add collision impulse and friction
		if (p[i].y <= 0)
		{
			glm::vec3 normal = glm::vec3(0.0f,1.0f,0.0f);
			glm::vec3 vDiff = (v[i]) - glm::vec3(0,0,0);
			
			glm::vec3 vDiff1 = normal * glm::dot(normal, vDiff); // vDiff composant in normal direction
			glm::vec3 vDiff2 = vDiff - vDiff1; // vDiff composant orthogonal to normal direction

			glm::vec3 collisionImpulse = -(pArg.elasticity + 1) * vDiff1 * pArg.mass;
			glm::vec3 frictionImpulse = -pArg.dynamicFriction * vDiff2 * pArg.mass;
			
			F[i] += (collisionImpulse + frictionImpulse) / dt;
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
