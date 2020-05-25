#version 330 core
uniform sampler2D Texture0;
uniform sampler2D shadowDepth;
uniform vec3 lightDir;

out vec4 Outcolor;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec4 fPosLS;
   vec3 vColor;
} in_struct;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {
    if(in_struct.fPos.y < lightDir.y - 750){
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

void main() {

  float Shade = 0.0;
  float amb = 0.3;
  
  vec4 BaseColor = vec4(in_struct.vColor, 1);
  vec4 texColor0 = texture(Texture0, in_struct.vTexCoord);


  Shade = TestShadow(in_struct.fPosLS);

  Outcolor = amb*(texColor0) + (1.0-Shade)*texColor0;//*BaseColor;
}

