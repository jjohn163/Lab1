#include "ConfettiParticle.h"
#include <iostream>

const float initial_speed = 30.0f;
const float disappear_speed = 0.95;

ConfettiParticle::ConfettiParticle(string name, vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale)
: Particle(name, position, rotation, velocity, gravityEffect, lifeLength, scale) {
	initial_direction = normalize(velocity);
}

bool ConfettiParticle::update(float deltaTime) {
	float time_factor = (life_length - elapsed_time) / life_length;
	velocity = initial_direction * initial_speed * (time_factor * time_factor);

	scale = scale * disappear_speed;

	return Particle::update(deltaTime);
}
