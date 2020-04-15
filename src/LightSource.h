#pragma once
#ifndef _LIGHTSOURCE_H_
#define _LIGHTSOURCE_H_

#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

class Program;

class LightSource
{
public:
	LightSource(glm::vec3 light, glm::vec3 pos);
	virtual ~LightSource();
	glm::vec3 intensity;
	glm::vec3 position;

};

#endif