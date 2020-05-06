#pragma once

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include "Enitity.h"
#include "Program.h"
#include "GLSL.h"


#define P_TEX_WIDTH  8    // Particle texture dimensions
#define P_TEX_HEIGHT 8


struct Particle {
	vec3 position;
	float rotation;
	vec3 velocity;
	float gravityEffect;
	float lifeLength;
	float scale;

	float elapsedTime = 0;

	Particle(vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale) {
		this->position = position;
		this->velocity = velocity;
		this->gravityEffect = gravityEffect;
		this->lifeLength = lifeLength;
		this->scale = scale;
	}

	bool update(float deltaTime) {

		float GRAVITY = 17.0f;
		velocity.y += GRAVITY * gravityEffect * deltaTime;	// update velocity
		vec3 delta_pos = velocity * deltaTime;	// update pos
		position += delta_pos;
		elapsedTime += deltaTime;	// update elapsed time

		return elapsedTime < lifeLength;
	}
};


class ParticleSystem {
public:

	// Constructor
	ParticleSystem(string tex_file_path, string vs_file_path, string fs_file_path);

	// Per frame
	void updateParticles(float delta_frame);
	void render(float delta_time, mat4 V, vec3 camera);
	void setProjection(mat4 P);

	static vector<Particle>& getParticles() { return particles; }

	Particle * newParticle(vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale);

private:
	static vector<Particle> particles;

	// Render State
	std::shared_ptr<Program> prog;	// Program
	string vs_file_path;
	string fs_file_path;

	unsigned int VAO;				// Mesh

	GLuint particle_tex_id;			// Texture
	string tex_file_path;
	unsigned char *data;
};


#endif // PARTICLESYSTEM_H
