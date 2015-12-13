#include <vector>

#include <armadillo>
#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/type_ptr.hpp"
#include <maya/MGlobal.h>

class ParticleSystem
{
public:
	ParticleSystem(std::vector<glm::vec3> positions);
	~ParticleSystem();

	void stepPhysics(float dt);
	void matchShape(float dt);
	glm::vec3 getPosition(int i);
private:
	void updateForces(float dt);
	void updateVelocities(float dt);
	void updatePositions(float dt);
	glm::vec3 computeCOM();
	glm::mat3 mat3Sqrt(glm::mat3 m);

	std::vector<glm::vec3> p0; // Initial positions
	std::vector<glm::vec3> p;
	std::vector<glm::vec3> v;
	std::vector<glm::vec3> F;
	std::vector<glm::vec3> g;
	glm::vec3 initialCenterOfMass;
};