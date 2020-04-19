#include "CollectionSphere.h"

using namespace glm;

CollectionSphere::CollectionSphere(vec3 position, vec3 direction, vec3 speed, float radius, bool isMoving=true)
{
	position = position;
	direction = direction;
	speed = speed;
	radius = radius;
	isMoving = isMoving;
}

vec3 CollectionSphere::getPosition() {
	return this->position;
}
vec3 CollectionSphere::getDirection() {
	return this->direction;
}
vec3 CollectionSphere::getSpeed() {
	return this->speed;
}
float CollectionSphere::getRadius() {
	return this->radius;
}
bool CollectionSphere::isMoving() {
	return this->moving;
}
void CollectionSphere::setPosition(vec3 newPos) {
	this->position = newPos;
}
void CollectionSphere::setDirection(vec3 newDir) {
	this->direction = newDir;
}
void CollectionSphere::setSpeed(vec3 newSpeed) {
	this->speed = newSpeed;
}
void CollectionSphere::toggleMoving() {
	this->moving = !this->moving;
}

CollectionSphere::~CollectionSphere()
{
}
