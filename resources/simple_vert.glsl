#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS;

out vec3 fragNor;
out vec3 fragPos;
out vec2 vTex;

out vec3 fPos;
out vec4 fPosLS;


void main()
{
	gl_Position = P * V *M* vertPos;
	fragNor = (V* M * vec4(vertNor, 0.0)).xyz;
	fragPos = vec3(V* M*vertPos);
	vTex = vertTex;
    fPos = (M*vertPos).xyz;
    fPosLS = LS * M * vec4(vertPos.xyz, 1.0);
}
