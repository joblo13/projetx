//[VERTEX SHADER]
#version 330
 
in vec3 InVertex;
in vec2 InUV;
in vec3 InNormal;

//Animated
in ivec4 InBoneIDs;
in vec4 InWeights;
in mat4 InModelview;

//hpbar
in float InHealthPercent;
in float InArmorPercent;

//Terrain
in int InColor;
in int InTextureNb;
in float InDamage;
in int InRenforcement;

#define TERRAIN 0
#define STATIC_MESH 1
#define ANIMATED_MESH 2
#define SKYBOX 3
#define DEFAULT_WHITE_LINE 4
#define INSTANCIATE_WHITE_LINE 5
#define HITPOINT_BAR 6
uniform int InExecutionType;

smooth out vec2 UV;                  
smooth out vec3 Normal;   
smooth out vec3 WorldSpacePos;  

//hpbar
smooth out float HealthColor;
smooth out float ArmorColor;

//Terrain
flat out vec4 Color; //a = comportement de l'affichage
flat out int TextureNb;
flat out float Damage;
flat out int Renforcement;

//Animated
uniform samplerBuffer AnimMatrices;

uniform int numBones;

uniform mat4 InProjectionModelview;
uniform mat4 InWorld;

void main()
{
	Color = vec4((float((InColor >> 24) & 0x000000FF))/255.0, (float((InColor >> 16) & 0x000000FF))/255.0, 
			(float((InColor >> 8) & 0x000000FF))/255.0, ((InColor) & 0x000000FF));
	if(InExecutionType == TERRAIN)
	{
		gl_Position = InProjectionModelview * vec4(InVertex, 1.0);
		TextureNb = InTextureNb;
		Damage = InDamage;
		Renforcement = InRenforcement;
	}
	else if(InExecutionType == ANIMATED_MESH)
	{
		mat4 matrix;
		int matrixNum;
		mat4 boneTransform = mat4(0.0);
		
		for(int i=0; i < 4; ++i)
		{
			matrixNum = gl_InstanceID * numBones + InBoneIDs[i];
			matrix = mat4(texelFetch(AnimMatrices, (matrixNum * 4 + 0)),
							texelFetch(AnimMatrices, (matrixNum * 4 + 1)),
							texelFetch(AnimMatrices, (matrixNum * 4 + 2)),
							texelFetch(AnimMatrices, (matrixNum * 4 + 3)));
			boneTransform += matrix * InWeights[i];
		}
		
		gl_Position = InProjectionModelview * InModelview * boneTransform * vec4(InVertex, 1.0);
	}
	else if(InExecutionType == STATIC_MESH || InExecutionType == HITPOINT_BAR)
	{
		gl_Position = InProjectionModelview * InModelview * vec4(InVertex, 1.0);
		if(InExecutionType == HITPOINT_BAR)
		{
			//gauche 0, 3, 4
			//droite 1, 2, 5
			HealthColor = 1.f - InHealthPercent - ((gl_VertexID == 0 || gl_VertexID == 3 || gl_VertexID == 4)?1.f:0.f);
			ArmorColor = 1.f - InArmorPercent - ((gl_VertexID == 0 || gl_VertexID == 3 || gl_VertexID == 4)?1.f:0.f);
		}
	}
	else
		gl_Position = InProjectionModelview * vec4(InVertex, 1.0);	
    UV      = InUV;                  
    Normal        = (InWorld * vec4(InNormal, 0.0)).xyz;   
    WorldSpacePos = (InWorld * vec4(InVertex, 1.0)).xyz;  
}