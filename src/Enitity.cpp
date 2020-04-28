#include "Enitity.h"

const float GRAVITY = -17.0f;

Entity::Entity(glm::vec3 pos, glm::vec3 scl, glm::vec3 rot, bool mov)
{
	position = pos;
	scale = scl;
	rotation = rot;
	velocity = vec3(0.0, 0.0, 0.0);
	moving = mov;
}
void Entity::updatePosition(float deltaTime) {
	velocity.y += GRAVITY * deltaTime;
	vec3 change = deltaTime * velocity;
	position += change;
	for (shared_ptr<Collider> collider : colliders) {
		collider->updatePosition(change);
	}
}


Entity::~Entity()
{
}