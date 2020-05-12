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
#include "Particle.h"
#include "GLSL.h"

#define P_TEX_WIDTH  8    // Particle texture dimensions
#define P_TEX_HEIGHT 8

class ParticleSystem {
public:

	// Constructor
	ParticleSystem(string resourceDir, string vs_file_path, string fs_file_path);

	// Add new particle to particle system
	Particle* addNewParticle(string particle_name, string particle_type, vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale);
	// Per frame
	void updateParticles(float delta_frame);
	void render(float delta_time, mat4 V, vec3 camera);
	void setProjection(mat4 P);

	static vector<Particle*>& getParticles() { return particles; }

	//Particle * newParticle(string tex_file_path, vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale);

private:

	string get_particle_resource(string particle_name);

	static vector<Particle*> particles;

	// Render State
	std::shared_ptr<Program> prog;	// Program
	string resource_dir;
	string vs_file_path;
	string fs_file_path;

	unsigned int VAO;				// Mesh

	std::map<string, GLuint> particle_dictionary;
	GLuint number_of_particles = 0;
	unsigned char *data;
};


#endif // PARTICLESYSTEM_H
