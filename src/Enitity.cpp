#include "Enitity.h"



Entity::Entity(glm::vec3 pos, glm::vec3 scl, glm::vec3 rot, bool mov)
{
	position = pos;
	scale = scl;
	rotation = rot;
	velocity = vec3(0.0, 0.0, 0.0);
	moving = mov;
}


Entity::~Entity()
{
}