#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D prevTex;
uniform sampler2D genericTex;
uniform float alpha;

void make_kernel(inout vec4 n[9], sampler2D tex, ivec2 coord) {
  n[0] = texelFetch(tex, coord + ivec2(-1, -1), 0);
  n[1] = texelFetch(tex, coord + ivec2( 0, -1), 0);
  n[2] = texelFetch(tex, coord + ivec2( 1, -1), 0);
  n[3] = texelFetch(tex, coord + ivec2(-1,  0), 0);
  n[4] = texelFetch(tex, coord, 0);
  n[5] = texelFetch(tex, coord + ivec2( 1,  0), 0);
  n[6] = texelFetch(tex, coord + ivec2(-1,  1), 0);
  n[7] = texelFetch(tex, coord + ivec2( 0,  1), 0);
  n[8] = texelFetch(tex, coord + ivec2( 1,  1), 0);
}

void main(){
	mat3 I;
	mat3 C;
	float cnv[9];
	vec3 avg;
	vec3 sample;

	vec3 texColor = texture( genericTex, texCoord ).rgb;	
	texColor = texColor - mod(texColor, 0.2);

	vec4 n[9];
	ivec2 px = ivec2(gl_FragCoord.xy);
	make_kernel(n, genericTex, px);

	vec4 sobel_edge_h = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
	vec4 sobel_edge_v = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
	vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));
	
	//If color border meets threshold then make it brown
	if(sobel.r + sobel.g + sobel.b > 1) {
		color = vec4(vec3(.4, .25, .12), 1.0);
	} else {
		vec4 color_current = texture(genericTex, texCoord);
		vec4 color_prev = texture(prevTex, texCoord);
		
		//Color binning
		color_current =  color_current - mod(color_current, 0.2);
		color_prev =  color_prev - mod(color_prev, 0.2);

		color = color_current * alpha + (color_prev * (1-alpha));
  }
}
