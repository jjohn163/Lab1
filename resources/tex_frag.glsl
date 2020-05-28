#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D posBuf;
uniform sampler2D norBuf;
uniform sampler2D colorBuf;
uniform sampler2D lightBuf;
//ignored for now
uniform vec3 Ldir;

/* just pass through the texture color we will add to this next lab */
void main(){
   vec3 geom = texture( posBuf, texCoord ).rgb;
   vec3 nor = texture(norBuf, texCoord).rgb;
   vec3 kd = texture(colorBuf, texCoord).rgb;
   vec3 lightN = 2.0 * (texture(lightBuf, texCoord).rgb) + vec3(-1);

   // diffuse coefficient
   float dC = max(0, dot(normalize(nor), normalize(lightN)));
   color = vec4(dC * kd +  0.5 * kd, 1.0);

/*   if (abs(tColor.r) > 0.01 || abs(tColor.g) > 0.01)
   	color = vec4(0.9, 0.9, 0.9, 1.0);*/
    color.rgb = kd;
}
