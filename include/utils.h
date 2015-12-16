#include <string>

#include <armadillo>

#include <maya/MVector.h>


#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/type_ptr.hpp"
#include "../ext/glm/gtx/transform.hpp"

// Debug to string functions
std::string to_string(arma::fmat m);
std::string to_string(arma::fvec v);
std::string to_string(glm::vec3 v);
std::string to_string(glm::mat3 m);

// Conversions
glm::vec3 to_glm(arma::fvec v);
glm::mat3 to_glm(arma::fmat M);
glm::mat3 to_glm3x9(arma::fmat M);
glm::vec3 to_glm(MVector v);

