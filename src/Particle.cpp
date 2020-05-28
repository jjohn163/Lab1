#include "Particle.h"


bool Particle::update(float deltaTime) {

	vec3 delta_pos = velocity * deltaTime;	// update pos
	position += delta_pos;
	elapsed_time += deltaTime;	// update elapsed time

	position -= vec3(0, gravity_effect, 0) * deltaTime;

	return elapsed_time < life_length;
}