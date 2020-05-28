#pragma once

#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Enitity.h"
#include "Program.h"
#include "GLSL.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <stdlib.h>

#define TEX_WIDTH  8    // Particle texture dimensions
#define TEX_HEIGHT 8

class Billboard {
public: 
	Billboard(string resourceDir, string tex_file, string vs_file_path, string fs_file_path);
	void render(float delta_time, mat4 V, vec3 camera);
	void setProjection(mat4 P);

private:
	// Render State
	std::shared_ptr<Program> prog;	// Program
	string resource_dir;
	string vs_file_path;
	string fs_file_path;	

	GLuint tex_id;
	string tex_file;

	unsigned int VAO;				// Mesh
};

#endif // !BILLBOARD_H
