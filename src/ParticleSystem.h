#pragma once

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <glm/gtc/type_ptr.hpp>
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

class ParticleSystem {
public:
	struct Particle {
		vec3 position;
		vec3 velocity;
		float gravityEffect;
		float lifeLength;
		float scale;

		float elapsedTime = 0;

		Particle(vec3 position, vec3 velocity, float gravityEffect, float lifeLength, float scale) {
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

	// Constructor
	ParticleSystem(string tex_file_path, string vs_file_path, string fs_file_path) {
		this->tex_file_path = tex_file_path;
		this->vs_file_path = vs_file_path;
		this->fs_file_path = fs_file_path;
		
		// Set up mesh and attribute properties
		unsigned int VBO;
		float particle_quad[] = {
			-0.5f, 0.5f, -0.5f, -0.5f, 
			0.5f, 0.5f, 0.5f, -0.5f, 
			0.0f, 0.0f, 0.0f, 0.0f,

			-0.5f, 0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 1.0f, 0.0f
		};
		CHECKED_GL_CALL(glGenVertexArrays(1, &this->VAO));
		CHECKED_GL_CALL(glGenBuffers(1, &VBO));
		CHECKED_GL_CALL(glBindVertexArray(this->VAO));
		// fill mesh buffer
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW));
		// set mesh attributes
		CHECKED_GL_CALL(glEnableVertexAttribArray(0));
		CHECKED_GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));
		CHECKED_GL_CALL(glBindVertexArray(0));
	}

	// Render
	void update(float delta_time);
	void setViewProjection(mat4 V, mat4 P, vec3 camPos) {
		prog->bind();

		mat4 M = translate(mat4(1.0), -camPos);
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V)));
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P)));
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M)));
		prog->unbind();
	};


private:
	const float VERTS [8] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f };


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
