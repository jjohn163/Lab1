#pragma once
#include "Collider.h"
class SphereCollider :
	public Collider
{
public:
	SphereCollider(glm::vec3 pos, float rad);
	~SphereCollider();
	std::pair<bool, glm::vec3> isColliding(Collider* other, glm::vec3 velocity);
private:
	float radius;
};

