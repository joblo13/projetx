//[VERTEX SHADER]
#version 330
 
in vec3 InVertex;
 
uniform mat4 InProjectionModelview;
 
void main()
{
	gl_Position = InProjectionModelview * vec4(InVertex, 1.0);
}