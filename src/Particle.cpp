#include "Particle.h"


bool Particle::update(float deltaTime) {

	vec3 delta_pos = velocity * deltaTime;	// update pos
	position += delta_pos;
	elapsed_time += deltaTime;	// update elapsed time

	return elapsed_time < life_length;
}