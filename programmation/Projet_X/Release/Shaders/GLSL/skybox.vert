//[VERTEX SHADER]
#version 330
 
in vec3 InVertex;
in vec2 InUV;

smooth out vec2 UV;
 
uniform mat4 InProjectionModelview;
 
void main()
{
	//gl_Position.w = 0;
	gl_Position = InProjectionModelview * vec4(InVertex, 1.0);
	gl_Position.z = gl_Position.w - 0.00001;
	UV = InUV;
}