#pragma once

#include "Particle.h"

class ConfettiParticle : 
	public Particle 
{
public:
	ConfettiParticle(string name, vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale);
	bool update(float deltaTime) override;
private:
	vec3 initial_direction;
};
