#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D prevTex;
uniform sampler2D genericTex;
uniform float alpha;

void main(){
	vec4 color_current = texture(genericTex, texCoord);
    vec4 color_prev = texture(prevTex, texCoord);

    color = color_current * alpha + (color_prev * (1-alpha));
}
