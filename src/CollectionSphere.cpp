#include "CollectionSphere.h"
#include "TimeManager.h"
#include <iostream>

using namespace glm;
using namespace std;

CollectionSphere::CollectionSphere(vec3 pos, vec3 dir, float spd, float rad, bool mov)
{
	position = pos;
	direction = dir;
	speed = spd;
	radius = rad;
	moving = mov;
}

bool didCollide(vector<shared_ptr<CollectionSphere>> &collectionSpheres, vec3 pos, float radius, float eye_radius, vec3 eye) {
	//check if collides with other collector spheres
	for (int i = 0; i < collectionSpheres.size(); i++) {
		if (glm::distance(collectionSpheres[i]->getPosition(), pos)
			< (collectionSpheres[i]->getRadius() + radius)) {
			return true;
		}
	}
	//check if collides with the player
	if (glm::distance(eye, pos)
		< (eye_radius + radius)) {
		return true;
	}
	return false;
}

void CollectionSphere::printScore(vector<shared_ptr<CollectionSphere>> &collectionSpheres) {
	cout << "Collected: " << collected << endl;
	cout << "Alive: " << collectionSpheres.size() - collected << endl;
}

void CollectionSphere::spawnEnemies(vector<shared_ptr<CollectionSphere>> &collectionSpheres, float eye_radius, vec3 eye) {
	long long currentTime = TimeManager::Instance()->GetTime();
	if (((currentTime - lastSpawn) > SPAWN_DELAY) && (collectionSpheres.size() < 50)) {
		int xpos;
		int zpos;
		int radius = static_cast<int>(SPHERE_RADIUS);
		do {
			xpos = (rand() % (WIDTH - radius) * 2) - (WIDTH - radius);
			zpos = (rand() % (HEIGHT - radius) * 2) - (HEIGHT - radius);
		} while (didCollide(collectionSpheres, vec3(xpos, 0, zpos), SPHERE_RADIUS, eye_radius, eye));

		vec3 direction = normalize(vec3(rand(), 0, rand()));
		shared_ptr<CollectionSphere> temp = make_shared<CollectionSphere>(vec3(xpos, 0, zpos), direction, SPHERE_SPEED, SPHERE_RADIUS, true);
		collectionSpheres.push_back(temp);
		lastSpawn = currentTime;
		printScore(collectionSpheres);
	}
}

void CollectionSphere::manageCollisions(vector<shared_ptr<CollectionSphere>> &collectionSpheres, float eye_radius, vec3 eye) {
	for (int i = 0; i < collectionSpheres.size(); i++) {

		//collisions with boundary
		vec3 currentPos = collectionSpheres[i]->getPosition();
		float radius = collectionSpheres[i]->getRadius();
		vec3 direction = collectionSpheres[i]->getDirection();
		if ((currentPos.x + radius > WIDTH) || (currentPos.x - radius < -WIDTH)) {
			direction.x = -direction.x;
		}
		if ((currentPos.z + radius > HEIGHT) || (currentPos.z - radius < -HEIGHT)) {
			direction.z = -direction.z;
		}
		collectionSpheres[i]->setDirection(direction);

		//collisions with other spheres
		for (int j = i + 1; j < collectionSpheres.size(); j++) {
			if (glm::distance(collectionSpheres[i]->getPosition(), collectionSpheres[j]->getPosition())
				< (collectionSpheres[i]->getRadius() + collectionSpheres[j]->getRadius())) {
				//assuming perfect collision 

				vec3 direction = normalize(vec3(collectionSpheres[j]->getPosition().x - collectionSpheres[i]->getPosition().x,
					0,
					collectionSpheres[j]->getPosition().z - collectionSpheres[i]->getPosition().z));
				if (collectionSpheres[i]->isMoving()) {
					collectionSpheres[i]->setDirection(-direction);
				}
				if (collectionSpheres[j]->isMoving()) {
					collectionSpheres[j]->setDirection(direction);
				}
			}
		}

		//collisions with player
		if (glm::distance(eye, collectionSpheres[i]->getPosition()) < (eye_radius + collectionSpheres[i]->getRadius())
			&& collectionSpheres[i]->isMoving()) {
			collectionSpheres[i]->toggleMoving();
			collected++;
			printScore(collectionSpheres);
		}
	}
}

void CollectionSphere::updateEnemies(vector<shared_ptr<CollectionSphere>> &collectionSpheres) {
	float deltaTime = TimeManager::Instance()->DeltaTime();
	for (shared_ptr<CollectionSphere> sphere : collectionSpheres) {
		if (sphere->isMoving()) {
			vec3 newPos = sphere->getPosition() + (sphere->getDirection() * sphere->getSpeed() * deltaTime);
			sphere->setPosition(newPos);
		}
	}
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
