#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in vec3 fragPos;
in vec3 fragNor;
in vec2 vTex;
in vec3 fPos;
in vec4 fPosLS;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform sampler2D Texture0;
uniform sampler2D shadowDepth;
uniform vec3 LightPos;


float TestShadow(vec4 LSfPos) {
    if(fPos.y < LightPos.y - 750){
        return 0.0;
    }
    float bias = 0.005;
    float lightDepth = 0.0;
    float percentShadow = 0.0;
	//1: shift the coordinates from -1, 1 to 0 ,1
    vec3 shifted = LSfPos.xyz/LSfPos.w;
    shifted = 0.5*(shifted.xyz +vec3(1.0));
    vec2 texelScale = 1.0 / textureSize(shadowDepth, 0);
    float curD = shifted.z - bias;

    for (int i = -2; i<=2; i++){
        for(int j = -2; j <= 2; j++){
            lightDepth = texture(shadowDepth, shifted.xy + vec2(i,j) * texelScale).r;
            if(curD > lightDepth){
                percentShadow += 1.0;
            }
        }
    }
    return percentShadow = percentShadow/25.0;
}

void main()
{


    float Shade = 0.0;
    float amb = 0.3;
  
    Shade = TestShadow(fPosLS);


    vec4 texColor = texture(Texture0, vTex)
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(fragNor);

    gColorSpec = amb*(texColor) + (1.0-Shade)*texColor;

} 
