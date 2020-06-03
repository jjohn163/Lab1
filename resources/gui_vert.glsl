
#version 330 core
layout (location = 0) in vec4 vertPos; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 offset;
uniform vec4 color;
uniform vec2 scale;

void main()
{
    TexCoords = vertPos.zw;
    ParticleColor = color;

   
    vec2 vpos = vertPos.xy;// + vec2(-9, -1); //+ vec2(-0.5, 0.5);
    vpos.x *= scale.x;
    vpos.y *= scale.y;

	vec4 pos = M * vec4(vec3(vpos, 0.0), 1.0);
    gl_Position = P * V * pos;
}