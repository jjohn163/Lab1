#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;

uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );

uniform float unblurredRadius;

void main(){
	vec3 texColor = texture( texBuf, texCoord ).rgb;	

	float unblurred = abs(unblurredRadius);

	float middleness = clamp(pow(8*texCoord.x - 4, 2) + pow(8 * texCoord.y - 4, 2) - unblurred, 0, 1);

	color = vec4(texColor, 1) * (1 - middleness);

	color += vec4(texColor*weight[0], 1) * middleness;

	for (int i=1; i <5; i ++) 
	{
		color += vec4(texture( texBuf, texCoord + vec2(offset[i], 0.0)/512.0 ).rgb, 1) * weight[i] * middleness;
		color += vec4(texture( texBuf, texCoord - vec2(offset[i], 0.0)/512.0 ).rgb, 1) * weight[i] * middleness;
	}
}
