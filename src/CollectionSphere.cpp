#include "CollectionSphere.h"

using namespace glm;

CollectionSphere::CollectionSphere(vec3 pos, vec3 dir, float spd, float rad, bool mov)
{
	position = pos;
	direction = dir;
	speed = spd;
	radius = rad;
	moving = mov;
}

vec3 CollectionSphere::getPosition() {
	return this->position;
}
vec3 CollectionSphere::getDirection() {
	return this->direction;
}
float CollectionSphere::getSpeed() {
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
void CollectionSphere::setSpeed(float newSpeed) {
	this->speed = newSpeed;
}
void CollectionSphere::toggleMoving() {
	this->moving = false;
}

CollectionSphere::~CollectionSphere()
{
}
