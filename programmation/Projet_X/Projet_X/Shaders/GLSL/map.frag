//[FRAGMENT SHADER]
#version 330
 
smooth in vec2 UV;
 
out vec4 FragColor;
 
uniform sampler2D MapTexture;
 
void main()
{
	FragColor = vec4(texture2D( MapTexture, vec2(UV.s, UV.t)).rgb, 1);
}