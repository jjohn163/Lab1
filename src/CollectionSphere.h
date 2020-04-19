#pragma once
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

class CollectionSphere
{
public:
	CollectionSphere::CollectionSphere(vec3 position, vec3 direction, float speed, float radius, bool moving);
	vec3 getPosition();
	vec3 getDirection();
	float getSpeed();
	float getRadius();
	bool isMoving();
	void setPosition(vec3 newPos);
	void setSpeed(float newSpeed);
	void setDirection(vec3 newDir);
	void toggleMoving();
	~CollectionSphere();

private:
	vec3 position;
	vec3 direction;
	float speed;
	float radius;
	bool moving;
};

