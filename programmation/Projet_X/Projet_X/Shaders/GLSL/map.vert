//[VERTEX SHADER]
#version 330
 
in vec3 InVertex;
in vec2 InUV;

smooth out vec2 UV;
 
uniform mat4 InUVMatrix;
 
void main()
{
	gl_Position = vec4(InVertex.x, InVertex.y, InVertex.z, 1.0);
	vec4 tempUV = InUVMatrix * vec4(InUV, 1.0, 1.0);
	UV = vec2(tempUV.x, tempUV.y);
}