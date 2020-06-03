#pragma once

#ifndef  GUI_RENDERER_H
#define GUI_RENDERER_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include "Enitity.h"
#include "Program.h"
#include "Particle.h"
#include "GLSL.h"
#include "GuiTexture.h"

class GuiRenderer {
public:
	GuiRenderer(string resource_dir,string vs_file_path,string fs_file_path) {
		this->resource_dir = resource_dir;
		this->vs_file_path = resource_dir + vs_file_path;
		this->fs_file_path = resource_dir + fs_file_path;
	}
	
	void init();
	void render(vector<GuiTexture*> guis, float delta_frame, mat4 V, mat4 P, vec3 camera);
	mat4 faceCamera(mat4 model, mat4 view);

private:
	std::shared_ptr<Program> prog;	// Program
	string resource_dir;
	string vs_file_path;
	string fs_file_path;

	GLuint quadVAO;

};
#endif // ! GUI_RENDERER_H
