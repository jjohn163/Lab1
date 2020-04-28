#include "Enitity.h"

const float GRAVITY = -17.0f;
const float AIR_RESISTANCE_FACTOR = 0.985;

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
	velocity.x *= AIR_RESISTANCE_FACTOR;
	velocity.z *= AIR_RESISTANCE_FACTOR;
	vec3 change = deltaTime * velocity;
	position += change;
	for (shared_ptr<Collider> collider : colliders) {
		collider->updatePosition(change);
	}
}


Entity::~Entity()
{
}