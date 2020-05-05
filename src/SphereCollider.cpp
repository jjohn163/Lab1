#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "OBBCollider.h"

#include <iostream>

using namespace std;

SphereCollider::SphereCollider(glm::vec3 pos, float rad)
	:Collider(pos)
{
	radius = rad;
}
std::pair<bool, glm::vec3> SphereCollider::isColliding(Collider* other, glm::vec3 velocity) {
	vec3 dir = normalize(velocity);
	float speed = length(velocity) * 0.8f; //dampening
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(other)) {
		if (length(position - sphere->position) <= (radius + sphere->radius)) {
			float moving_towards = dot(velocity, position - sphere->position);
			if (moving_towards < 0) {
				vec3 n = normalize(vec3(sphere->position.x - position.x, sphere->position.y - position.y, sphere->position.z - position.z));
				return { true, speed * (dir - 2.0f * dot(dir, n) * n) };
			}
		}
	}
	else if (PlaneCollider* plane = dynamic_cast<PlaneCollider*>(other)) {
		float dist = dot(position, plane->getNormal()) - plane->getD();
		if (fabs(dist) <= radius) {
			vec3 n = plane->getNormal();
			float moving_towards = dot(velocity, n);
			if (moving_towards < 0) {
				return { true, speed * (dir - 2.0f * dot(dir, n) * n) };
			}
		}
	}
	else if (OBBCollider* obb = dynamic_cast<OBBCollider*>(other)) {
		//obb.closestPtPointOBB()
		
		// Find point p on OBB closest to sphere center
		vec3 closest = obb->closestPtPointOBB(position);
		// Sphere and OBB intersect if the (squared) distance from sphere
		// center to point p is less than the (squared) sphere radius
		vec3 v = -(closest - position);

		if (dot(v, v) <= radius * radius) {
			float moving_towards = dot(velocity, v);
			if (moving_towards < 0) {
				v = normalize(v);
				return { true, speed * (dir - 2.0f * dot(dir, v) * v) };
			}
		}
	}
	return { false, vec3(0,0,0) };
}

SphereCollider::~SphereCollider()
{
}
