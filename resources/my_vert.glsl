#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPos;

out vec3 LightDir;
out vec3 normal;
out vec3 v;

void main()
{
    gl_Position = P * V * M * vertPos;
	LightDir = LightPos - (M * vertPos).xyz;
	normal = (M * vec4(vertNor, 0.0)).xyz;
	v = (-1.0)*(M * vertPos).xyz;
}
