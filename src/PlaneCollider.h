#pragma once
#include "Collider.h"
class PlaneCollider :
	public Collider
{
public:
	PlaneCollider(glm::vec3 a, glm::vec3 b, glm::vec3 c);//points are in ccw order
	~PlaneCollider();
	glm::vec3 getNormal();
	float getD();
private:
	glm::vec3 normal;
	float d;
};

