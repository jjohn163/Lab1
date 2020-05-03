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

#define P_TEX_WIDTH  8    // Particle texture dimensions
#define P_TEX_HEIGHT 8

class ParticleSystem {
public:

	struct Particle {
		glm::vec2 position, velocity;
		glm::vec4 color;
		float life;

		Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
	};

	// Constructor
	ParticleSystem(Program * prog, string tex_file_path, string vs_file_path, string fs_file_path) { 
		this->tex_file_path = tex_file_path;
		this->vs_file_path = vs_file_path;
		this->fs_file_path = fs_file_path;
	}

	// Initialization 
	void init();

	// Runs every frame
	void update(float delta_time);
	void setProjection(mat4 P) {
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	};

	// Get/Set
	void setParent(Entity * parent) { this->parent = parent; }
	Entity * getParent() { return parent; }


private:
	
	int max_particle_count;	
	std::vector<Particle> particles;

	// Particle system may be attached to a parent <Entity>
	Entity * parent;

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
