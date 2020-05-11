#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPos;

out vec3 LightDir;
out vec3 normal;
out vec3 v;
out vec2 vTex;

void main()
{
    gl_Position = P * V * M * vertPos;
	LightDir = LightPos - (M * vertPos).xyz;
	normal = (M * vec4(vertNor, 0.0)).xyz;
	v = (-1.0)*(M * vertPos).xyz;
	vTex = vertTex;
}
