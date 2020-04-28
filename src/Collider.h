#pragma once

#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

using namespace std;
using namespace glm;

class Collider
{
public:
	Collider(glm::vec3 pos);
	virtual ~Collider() {}
	virtual std::pair<bool, glm::vec3> isColliding(Collider* other, glm::vec3 velocity) { return { false, velocity }; }
	virtual void updatePosition(glm::vec3 change) { position += change; }
protected:
	glm::vec3 position;
};

