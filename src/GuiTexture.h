#pragma once
#ifndef GUI_H
#define GUI_H

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
#include "stb_image.h"

class GuiTexture {
public:
	
	GuiTexture() {}

	GuiTexture(GLuint texID, vec2 position, vec2 scale) {
		this->texID = texID;
		this->position = position;
		this->scale = scale;
	}

	GLuint loadTexture(string path);
	GLuint getTexID() { return texID; }
	vec2 getPosition() { return position; }
	vec2 getScale() { return scale; }
	void setPosition(vec2 newpos) { this->position = newpos; }
	void setScale(vec2 newscale) { this->scale= newscale; }

private:

	GLuint texID;
	vec2 position = vec2(0.0f, 0.0f);
	vec2 scale = vec2(1.0f, 1.0f);

};


#endif // !GUI_H
