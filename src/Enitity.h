#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Collider.h"

using namespace std;
using namespace glm;

class Entity
{
public:
	Entity(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, bool mov);
	~Entity();
	void updatePosition(float deltaTime);
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 velocity;
	vector<shared_ptr<Collider>> colliders;
	bool moving;
};

