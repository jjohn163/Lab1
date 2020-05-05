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
		glm::vec3 position, velocity;
		glm::vec4 color;
		float life;

		Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
	};

	// Constructor
	ParticleSystem(string tex_file_path, string vs_file_path, string fs_file_path) { 
		this->tex_file_path = tex_file_path;
		this->vs_file_path = vs_file_path;
		this->fs_file_path = fs_file_path;
		max_particle_count = 500;
	}

	// Initialization 
	void init();

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

	// Get/Set
	void setParent(std::shared_ptr<Entity> parent) { this->parent = parent; }
	std::shared_ptr<Entity> getParent() { return parent; }


private:
	
	int max_particle_count;	
	std::vector<Particle> particles;

	// Particle system may be attached to a parent <Entity>
	std::shared_ptr<Entity> parent;

	// Render State
	std::shared_ptr<Program> prog;	// Program
	string vs_file_path;
	string fs_file_path;
	
	unsigned int VAO;				// Mesh

	GLuint particle_tex_id;			// Texture
	string tex_file_path;
	unsigned char *data;


	unsigned int firstUnusedParticle();								// Returns the first dead particle
	void respawnParticle(Particle &particle, glm::vec3 offset);		// Respawn the dead particles 
};

#endif // PARTICLESYSTEM_H
