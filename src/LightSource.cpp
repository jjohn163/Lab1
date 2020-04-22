

#include "LightSource.h"
#include <iostream>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>
#include "GLSL.h"
#include "Program.h"

using namespace std;


LightSource::LightSource(glm::vec3 light, glm::vec3 pos) {
	intensity = (light);
	position = (pos);
}
	

LightSource::~LightSource()
{
}