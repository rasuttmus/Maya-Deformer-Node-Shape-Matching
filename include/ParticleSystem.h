#include <string>
#include <vector>

#include <maya/MGlobal.h>
#include <armadillo>

#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/type_ptr.hpp"
#include "../ext/glm/gtx/transform.hpp"

#include "../include/utils.h"

struct PhysicsArguments
{
	float
		mass,
		stiffness,
		elasticity,
		flappyness,
		deformation,
		staticFriction,
		dynamicFriction;

    glm::vec3
		gravity;        // Gravity vector
};

class ParticleSystem
{
public:
	ParticleSystem(std::vector<glm::vec3> positions, glm::vec3 initialVelocity);
	~ParticleSystem();

	void stepPhysics(float dt, PhysicsArguments pArg);
	void matchShape(float dt, PhysicsArguments pArg);
	glm::vec3 getPosition(int i);
private:
	void updateForces(float dt, PhysicsArguments pArg);
	void updateVelocities(float dt, PhysicsArguments pArg);
	void updatePositions(float dt);
	glm::vec3 computeCOM();

	std::vector<glm::vec3> p0; // Initial positions
	std::vector<glm::vec3> p;
	std::vector<glm::vec3> v;
	std::vector<glm::vec3> F;
	std::vector<glm::vec3> g;
	glm::vec3 initialCenterOfMass;
};