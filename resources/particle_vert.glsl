#version 330 core
layout (location = 0) in vec4 vertPos; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 P;
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = vertPos.zw;
    ParticleColor = color;
    gl_Position = P * vec4((vertPos.xy * scale) + offset, 0.0, 1.0);
}