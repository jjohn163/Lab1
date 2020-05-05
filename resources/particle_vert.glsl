
#version 330 core
layout (location = 0) in vec4 vertPos; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = vertPos.zw;
    ParticleColor = color;
	vec4 pos = M * vec4(vec3(vertPos.xy * scale, 0.0) + offset, 1.0);
    gl_Position = V * pos;
}