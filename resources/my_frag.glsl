#version 330 core 
in vec3 normal;
in vec3 LightDir;
in vec3 v;
in vec2 vTex;
out vec4 color;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
uniform vec3 MatAmb;
uniform sampler2D Texture0;

void main()
{
    vec4 texColor = texture(Texture0, vTex);
	vec3 LD = normalize(LightDir);
	vec3 n = normalize(normal);
	vec3 Vn = normalize(v);
	vec3 H = normalize(LD + Vn);
	color = vec4(MatAmb + MatDif * (max(0, dot(n,LD))) + MatSpec * pow(max(0, dot(n,H)),shine), 1.0);
//	color = vec4(MatAmb + MatDif * (max(0, dot(n,LD))), 1.0);
	color = texColor;
}
