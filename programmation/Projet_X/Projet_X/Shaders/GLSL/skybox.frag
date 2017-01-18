//[FRAGMENT SHADER]
#version 330
 
smooth in vec2 UV;
 
layout(location = 0) out vec4 FragColor;
 
uniform sampler2D TextureSampler;
 
void main()
{
	FragColor = vec4(texture2D( TextureSampler, UV).rgb, 1.0);
}