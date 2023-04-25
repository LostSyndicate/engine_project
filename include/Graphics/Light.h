//
// Created by losts_n1cs3jj on 11/5/2022.
//

#ifndef SM3_LIGHT_H
#define SM3_LIGHT_H

#include <glm/glm.hpp>

struct Light
{
	enum Type
	{
		POINT,
		SPOT,
		DIRECTIONAL
	};
	int offset;
	glm::mat4 matrix;
	int type;
	float range;
	float angle;
	glm::vec3 diffuse{ 1.f };
};

#endif //SM3_LIGHT_H
