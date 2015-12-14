#include "../include/utils.h"

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

glm::vec3 to_glm(arma::fvec v)
{
	return glm::vec3(v(0), v(1), v(2));
}

glm::mat3 to_glm(arma::fmat M)
{
	glm::mat3 M_glm;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			M_glm[i][j] = M(i,j);
	return M_glm;
}

glm::vec3 to_glm(MVector v)
{
	return glm::vec3(v[0], v[1], v[2]);
}
