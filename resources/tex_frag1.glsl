#version 330 core
uniform sampler2D Texture0;

in vec2 vTexCoord;
in float dCo;
out vec4 Outcolor;

void main() {
  vec4 texColor0 = texture(Texture0, vTexCoord);

	
  	//uncomment later on:  Outcolor = texColor0;
  
	//Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
	Outcolor = texColor0;
}

