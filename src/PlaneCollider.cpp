#include "PlaneCollider.h"
#include "Collider.h"


PlaneCollider::PlaneCollider(glm::vec3 a, glm::vec3 b, glm::vec3 c)
	:Collider(a)
{
	normal = normalize(cross(b - a, c - a));
	d = dot(normal, a);
}

glm::vec3 PlaneCollider::getNormal() {
	return normal;
}

float PlaneCollider::getD() {
	return d;
}
PlaneCollider::~PlaneCollider()
{
}
