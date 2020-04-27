#include "SphereCollider.h"
#include "PlaneCollider.h"


SphereCollider::SphereCollider(glm::vec3 pos, float rad)
	:Collider(pos)
{
	radius = rad;
}
std::pair<bool, glm::vec3> SphereCollider::isColliding(Collider* other, glm::vec3 velocity) {
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(other)) {
		if (length(position - sphere->position) <= (radius + sphere->radius)) {
			vec3 normal = normalize(vec3(sphere->position.x - position.x, sphere->position.y - position.y, sphere->position.z - position.z));
			float dp = dot(velocity, normal);
			vec3 direction = dp * normal;
			return { true, velocity + (2.0f * direction) };
		}
	}
	else if (PlaneCollider* plane = dynamic_cast<PlaneCollider*>(other)) {
		float dist = dot(position, plane->getNormal()) - plane->getD();
		if (fabs(dist) <= radius) {
			float dp = dot(velocity, plane->getNormal());
			vec3 direction = dp * plane->getNormal();
			return { true, velocity - 1.5f * direction };
		}
	}
	return { false, vec3(0,0,0) };
}

SphereCollider::~SphereCollider()
{
}
