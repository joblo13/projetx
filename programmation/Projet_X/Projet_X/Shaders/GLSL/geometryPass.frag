//[FRAGMENT SHADER]
#version 330

smooth in vec2 UV;                  
smooth in vec3 Normal;   
smooth in vec3 WorldSpacePos; 

//hpbar
smooth in float HealthColor;
smooth in float ArmorColor;

//Terrain
flat in vec4 Color;
flat in int TextureNb;
flat in float Damage;
flat in int Renforcement;

uniform sampler2D TextureSampler; 

#define TERRAIN 0
#define STATIC_MESH 1
#define ANIMATED_MESH 2
#define SKYBOX 3
#define DEFAULT_WHITE_LINE 4
#define INSTANCIATE_WHITE_LINE 5
#define HITPOINT_BAR 6

uniform int InExecutionType;
uniform int NbTextureTot;

uniform float InSpecularPower;
uniform float InMatSpecularIntensity;

layout(location = 0) out vec4 OutWorldSpacePos; // xyz = couleur diffuse a0 = maxLight
layout(location = 1) out vec4 OutDiffuse; // xyz = couleur diffuse a = specularPower
layout(location = 2) out vec4 OutNormal; // xyz = couleur diffuse a = specularIntensity
layout(location = 3) out vec3 OutTexCoord;

void main()
{
	vec4 tempColor = Color;
	if(InExecutionType == HITPOINT_BAR)
	{
		tempColor.xyz = vec3((HealthColor >= 0)?1.f:0.f, (HealthColor < 0)?1.f:0.f, 0.f);
		if(ArmorColor < 0)
			tempColor.xyz = vec3(0.f, 0.f, 1.f);
	}
		

	vec4 tempDiffuse;
	if(InExecutionType == TERRAIN)
	{		
		tempDiffuse = vec4(texture2D( TextureSampler, vec2((((1-sign(UV.s))/2) + (UV.s-int(UV.s)) + TextureNb)/NbTextureTot, UV.t)).rgb, 1.0f);
		
		if(Renforcement >= 1)
		{
			vec4 tempRenforcedDiffuse = vec4(texture2D( TextureSampler, vec2((((1-sign(UV.s))/2) + (UV.s-int(UV.s)) + 9)/NbTextureTot, UV.t)).rgb, 1.0f);
			
			if((int(tempColor.a) & 0x00000001) == 1  && tempRenforcedDiffuse.r*2 - (tempRenforcedDiffuse.g + tempRenforcedDiffuse.b) > 0.8f ||
			(int(tempColor.a) >> 1 & 0x00000001) == 1  && tempRenforcedDiffuse.g*2 - (tempRenforcedDiffuse.r + tempRenforcedDiffuse.b) > 0.8f ||
			(int(tempColor.a) >> 2 & 0x00000001) == 1 && tempRenforcedDiffuse.b*2 - (tempRenforcedDiffuse.r + tempRenforcedDiffuse.g) <= 0.8f)
				tempDiffuse = tempRenforcedDiffuse;
		}
	}
	else
		tempDiffuse = vec4(texture2D(TextureSampler, UV).xyz, 1.0f);
		//tempDiffuse = vec4(1.f,1.f,1.f, 1.0f);

	if((int(tempColor.a) & 0x00000001) == 1  && tempDiffuse.r*2 - (tempDiffuse.g + tempDiffuse.b) > 0.8f)
		discard;
	if((int(tempColor.a) >> 1 & 0x00000001) == 1  && tempDiffuse.g*2 - (tempDiffuse.r + tempDiffuse.b) > 0.8f)
		discard;
	if((int(tempColor.a) >> 2 & 0x00000001) == 1 && tempDiffuse.b*2 - (tempDiffuse.r + tempDiffuse.g) > 0.8f)
		discard;
	
 	
	OutDiffuse = tempDiffuse;
	
	if(InExecutionType == TERRAIN || InExecutionType == HITPOINT_BAR)
	{	
		if(tempColor.r < 1.f || tempColor.g < 1.f || tempColor.b < 1.f)
		{
			//http://fr.wikipedia.org/wiki/Niveau_de_gris
			float greyColor = (0.299 * OutDiffuse.x + 0.587 * OutDiffuse.y + 0.114 * OutDiffuse.z);
			OutDiffuse.x = greyColor;
			OutDiffuse.y = greyColor;
			OutDiffuse.z = greyColor;
			OutDiffuse *= vec4(tempColor.r, tempColor.g, tempColor.b, 1.0f);
		}
		if(InExecutionType == TERRAIN)
		{
			if(abs(0.5 - UV.t) + abs(0.5 - UV.s) < Damage)
				OutDiffuse *= vec4(texture2D( TextureSampler, vec2((((1-sign(UV.s))/2) + (UV.s-int(UV.s)) + 12.0)/NbTextureTot, UV.t)).rgb, 1.0f);
		}
	}
		
	OutDiffuse.a = InSpecularPower;
	
	float worldSpacePosA = 0.f;
	if((int(tempColor.a) >> 3 & 0x00000001) == 1)
		worldSpacePosA = 0x00000001;
	OutWorldSpacePos = vec4(WorldSpacePos, worldSpacePosA);
	OutNormal   = vec4(normalize(Normal), InMatSpecularIntensity);					
	OutTexCoord = vec3(UV, 0.0);
}