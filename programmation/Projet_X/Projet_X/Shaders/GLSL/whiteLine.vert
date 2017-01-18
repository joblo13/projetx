//[VERTEX SHADER]
#version 330
 
#define TERRAIN 0
#define STATIC_MESH 1
#define ANIMATED_MESH 2
#define SKYBOX 3
#define DEFAULT_WHITE_LINE 4
#define INSTANCIATE_WHITE_LINE 5
uniform int InExecutionType;
 
in vec3 InVertex;
in mat4 InModelview;

uniform mat4 InProjectionModelview;
 
void main()
{
	if(InExecutionType == DEFAULT_WHITE_LINE)
		gl_Position = InProjectionModelview * vec4(InVertex, 1.f);
	else if(InExecutionType == INSTANCIATE_WHITE_LINE)
		gl_Position = InProjectionModelview * InModelview * vec4(InVertex, 1.f);
}