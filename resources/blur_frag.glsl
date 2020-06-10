#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;

uniform float fTime;

uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );

uniform float unblurredRadius;

uniform mat3 G[9] = mat3[](
	1.0/(2.0*sqrt(2.0)) * mat3( 1.0, sqrt(2.0), 1.0, 0.0, 0.0, 0.0, -1.0, -sqrt(2.0), -1.0 ),
	1.0/(2.0*sqrt(2.0)) * mat3( 1.0, 0.0, -1.0, sqrt(2.0), 0.0, -sqrt(2.0), 1.0, 0.0, -1.0 ),
	1.0/(2.0*sqrt(2.0)) * mat3( 0.0, -1.0, sqrt(2.0), 1.0, 0.0, -1.0, -sqrt(2.0), 1.0, 0.0 ),
	1.0/(2.0*sqrt(2.0)) * mat3( sqrt(2.0), -1.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, -sqrt(2.0) ),
	1.0/2.0 * mat3( 0.0, 1.0, 0.0, -1.0, 0.0, -1.0, 0.0, 1.0, 0.0 ),
	1.0/2.0 * mat3( -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0 ),
	1.0/6.0 * mat3( 1.0, -2.0, 1.0, -2.0, 4.0, -2.0, 1.0, -2.0, 1.0 ),
	1.0/6.0 * mat3( -2.0, 1.0, -2.0, 1.0, 4.0, 1.0, -2.0, 1.0, -2.0 ),
	1.0/3.0 * mat3( 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 )
);

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

	vec3 texColor = texture( texBuf, texCoord ).rgb;	
	texColor = texColor - mod(texColor, 0.2);

	
	vec4 n[9];
  ivec2 px = ivec2(gl_FragCoord.xy);
  make_kernel(n, texBuf, px);

  vec4 sobel_edge_h = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
  vec4 sobel_edge_v = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
  vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));
  if(sobel.r + sobel.g + sobel.b > 3.6) {
	color = vec4(vec3(.4, .25, .12), 1.0);
  } else {

		float unblurred = abs(unblurredRadius);

		float middleness = clamp(pow(8*texCoord.x - 4, 2) + pow(8 * texCoord.y - 4, 2) - unblurred, 0, 1);

		color += vec4(texColor, 1) * (1 - middleness);

		color += vec4(texColor*weight[0], 1) * middleness;

		for (int i=1; i <5; i ++) 
		{
			color += vec4(texture( texBuf, texCoord + vec2(offset[i], 0.0)/512.0 ).rgb, 1) * weight[i] * middleness;
			color += vec4(texture( texBuf, texCoord - vec2(offset[i], 0.0)/512.0 ).rgb, 1) * weight[i] * middleness;
		}
	}
}
