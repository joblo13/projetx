#ifndef DEFINITION_H
#define DEFINITION_H

	enum{TERRAIN, STATIC_MESH, ANIMATED_MESH, SKYBOX, DEFAULT_WHITE_LINE, INSTANCIATE_WHITE_LINE, HITPOINT_BAR};

	#define MAX_RESSOURCES 4000000000

	#define DEFAULT_VIEW_ANGLE 70.0f
	#define RENDERING_DISTANCE_FAR 100.0f
	#define RENDERING_DISTANCE_NEAR 0.001f

	#define CHUNKLENGTH 16
	#define HALFSIDE 0.25f
	#define SIDE (HALFSIDE * 2.0f)
	#define ISOSURFACE_ERROR_CORRECTION (0.0000001f)
	#define JUMPINGSPEED 0.01f

	#define GRAVITY 0.00002f
	#define SKYBOX_ROTATION_SPEED 0.0003f

	#define MATH_PI 3.14159265358979323846f

	#define WIDTH 0
	#define HEIGHT 1
	#define LENGTH 2

	#define MAX_BONES 100

	#define BUFFER_OFFSET(i) ((void*)(i))

	#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

	//Type de donnée stockée dans la map des cubes
	enum CubeInfo{CUBE_TYPE, CUBE_MAX_HITPOINT, CUBE_HITPOINT, CUBE_RENFORCEMENT, CUBE_COLOR, 
		CUBE_HIGH_0F, CUBE_HIGH_1F, CUBE_HIGH_2F, CUBE_HIGH_3F, CUBE_HIGH_4F, CUBE_HIGH_5F, CUBE_HIGH_6F, CUBE_HIGH_7F,
		CUBE_DRAWING_INDEX, CUBE_NB_VERTEX, CUBE_DRAWING_INDEX_EXTRA_TOP, CUBE_NB_EXTRA_TOP, CUBE_CHANGED,
		NB_CUBE_PARAM};
	#define NB_UNSAVABLE_CUBE_ATTRIB 5

	enum{HEADER_GENERAL, HEADER_CHUNK, HEADER_CUBE};

	//Vertex, color
	//
	//SIZE : 4+4+4 +4 = 4*4 = 16 bytes
	//It's better to make it multiple of 32
	//32-16 = 16 bytes (of garbage should be added)
	//16/4 = 4 floats should be added
	struct VertexPosition
	{
		VertexPosition():x(0), y(0), z(0), color(0xFFFFFF04){}
		VertexPosition(float i_x, float i_y, float i_z):x(i_x), y(i_y), z(i_z), color(0xFFFFFF04){}
		VertexPosition(const VertexPosition& i_vertexPosition):x(i_vertexPosition.x), y(i_vertexPosition.y), z(i_vertexPosition.z), color(i_vertexPosition.color), 
		u(i_vertexPosition.u), v(i_vertexPosition.v)
		{
			normal[0] = i_vertexPosition.normal[0];
			normal[1] = i_vertexPosition.normal[1];
			normal[2] = i_vertexPosition.normal[2];
		}
		bool operator<(const VertexPosition& i_vertexPosition)const
		{
			if(x == i_vertexPosition.x)
			{
				if(y == i_vertexPosition.y)
				{
					if(z == i_vertexPosition.z)
						return color < i_vertexPosition.color;
					else
						return z < i_vertexPosition.z;
				}
				else
					return y < i_vertexPosition.y;
			}
			else
				return x < i_vertexPosition.x;
		}
		float	x, y, z;
		float	normal[3];
		int color;
		float u, v;
		void getRGBA(short& i_red, short& i_green, short& i_blue, short& i_alpha)const
		{
			i_red = (color >> 24) & 0xFF;
			i_green = (color >> 16) & 0xFF;
			i_blue = (color >> 8) & 0xFF;
			i_alpha = color & 0xFF;
		}
	};

	#include "OpenGLHandler.h"

	//Debug
	#define CAMERA_DEBUG 0



#endif

	//Physique
	#define MOVE_TRESHOLD 0.2f
	#define BASIC_FORCE 2.f
	#define SPEED_BOOST 5.f
	#define JUMP_FORCE 300.f
	#define VELOCITY_FACTOR_Y 1.f
	#define FALLING_REDUCTION 2.f