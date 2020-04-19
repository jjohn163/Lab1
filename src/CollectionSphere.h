#pragma once
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

class CollectionSphere
{
public:
	CollectionSphere(vec3 position, vec3 direction, vec3 speed, float radius, bool moving);
	vec3 getPosition();
	vec3 getDirection();
	vec3 getSpeed();
	float getRadius();
	bool isMoving();
	void setPosition(vec3 newPos);
	void setSpeed(vec3 newSpeed);
	void setDirection(vec3 newDir);
	void toggleMoving();
	~CollectionSphere();

private:
	vec3 position;
	vec3 direction;
	vec3 speed;
	float radius;
	bool moving;
};

