#pragma once

#include "Enitity.h" //Only including this to get string and vec3 (For some reason, I can't include it directly)
#include <iostream>

class Particle {
public:
	Particle(string name, vec3 position, float rotation, vec3 velocity, float gravityEffect, float life_length, float scale) {
		this->name = name;
		this->position = position;
		this->velocity = velocity;
		this->gravity_effect = gravityEffect;
		this->life_length = life_length;
		this->scale = scale;
	}
	virtual bool update(float deltaTime);

	string name;
	vec3 position;
	float rotation;
	float scale;

protected:
	vec3 velocity;
	float gravity_effect;
	float life_length;
	float elapsed_time = 0;
};
