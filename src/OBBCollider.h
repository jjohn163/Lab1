#pragma once
#include "Collider.h"
class OBBCollider :
	public Collider
{
public:
	OBBCollider(glm::vec3 center, glm::vec3 u[3], float e[3]);//points are in ccw order
	~OBBCollider();
	vec3 closestPtPointOBB(vec3 p);
private:
	glm::vec3 center;
	glm::vec3 u[3];
	float e[3];
};

