#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
    vec4 tcol = texture(tex, vertex_tex);
	
	// this is just set to something non-black for easier debugging
	gPosition = vec3(0.5,0.5,0.5);
	// this is just set to something non-black for easier debugging
	gNormal = vec3(0.5,0.5,0.5);
	//this is actually setting the skybox texture
	gColorSpec = tcol;
}
