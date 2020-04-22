#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>

using namespace std;
using namespace glm;

static const float SPHERE_RADIUS = 1.0;
static const int SPAWN_DELAY = 3000;
static const float SPHERE_SPEED = 5.0;
static const int WIDTH = 30;
static const int HEIGHT = 30;
static long long lastSpawn = 0;
static int collected = 0;


class CollectionSphere
{
public:
	CollectionSphere::CollectionSphere(vec3 position, vec3 direction, float speed, float radius, bool moving);
	static void manageCollisions(std::vector<shared_ptr<CollectionSphere>> &collectionSpheres, float eye_radius, vec3 eye);
	static void spawnEnemies(std::vector<shared_ptr<CollectionSphere>> &collectionSpheres, float eye_radius, vec3 eye);
	static void printScore(std::vector<shared_ptr<CollectionSphere>> &collectionSpheres);
	static void updateEnemies(std::vector<shared_ptr<CollectionSphere>> &collectionSpheres);
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

