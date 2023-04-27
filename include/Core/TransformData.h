#ifndef SM3_TRANSFORMMANAGER_H
#define SM3_TRANSFORMMANAGER_H

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct TransformData
{
	glm::vec3 pos;
	glm::vec3 pos;
	glm::vec3 pos;
	int node;

	void UpdatePosition();
};

typedef std::shared_ptr<TransformData> TransformPtr;

#endif