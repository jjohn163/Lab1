#include "FeatherParticle.h"
#include <iostream>

const float initial_speed = 15.0f;
const float disappear_speed = 0.95;

FeatherParticle::FeatherParticle(string name, vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale)
: Particle(name, position, rotation, velocity, gravityEffect, lifeLength, scale) {
	initial_direction = normalize(velocity);
}

bool FeatherParticle::update(float deltaTime) {
	float time_factor = (life_length - elapsed_time) / life_length;
	velocity = initial_direction * initial_speed * (time_factor * time_factor);

	if (life_length - elapsed_time < 1.0f) {
		scale = scale * disappear_speed;
	}

	return Particle::update(deltaTime);
}
