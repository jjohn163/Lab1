#pragma once
#include "OBBCollider.h"
#include "Collider.h"

//OBBCollider::OBBCollider(glm::vec3 a, glm::vec3 b, glm::vec3 c) :Collider(a) {
//	normal = normalize(cross(b - a, c - a));
//	d = dot(normal, a);
//}

OBBCollider::OBBCollider(glm::vec3 center, glm::vec3 u[3], float e[3]) :Collider(center) {
	this->center = center;
	this->u[0] = u[0];
	this->u[1] = u[1];
	this->u[2] = u[2];
	this->e[0] = e[0];
	this->e[1] = e[1];
	this->e[2] = e[2];
}

glm::vec3 OBBCollider::closestPtPointOBB(vec3 p) {
	vec3 d = p - center;
	
	vec3 q = center;

	for (int i = 0; i < 3; i++) {
		// ...project d onto that axis to get the distance
		// along the axis of d from the box center
		float dist = dot(d, u[i]);
		// If distance farther than the box extents, clamp to the box
		if (dist > e[i]) dist = e[i];
		if (dist < -e[i]) dist = -e[i];
		// Step that distance along the axis to get world coordinate
		q += dist * u[i];
	}

	return q;
}
OBBCollider::~OBBCollider() {

}
