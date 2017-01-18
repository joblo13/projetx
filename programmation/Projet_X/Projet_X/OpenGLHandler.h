#ifndef OPENGL_HANDLER_H
#define OPENGL_HANDLER_H

#ifdef _WIN32
	#include <GL/glew.h>
#elif __APPLE__
	#include <OpenGL/gl3.h>
#else
	#define GL3_PROTOTYPES 1
	#include <GL3/gl3.h>
#endif


// 0: pas de debug 1: debug 2: assert debug
#define OPENGL_DEBUG 1


#include <hash_map>

namespace OpenGLHandler
{
	void changeState(GLenum i_enum, bool i_target);
	void enable(GLenum i_enum);
	void disable(GLenum i_enum);

	void cullFace(GLenum i_enum);
	void blendEquation(GLenum i_enum);
	void blendFunc(GLenum i_enum1, GLenum i_enum2);
	void drawBuffer(GLenum i_enum);
	void drawBuffers(const std::vector<GLenum>& i_bufs);

	void loadState(GLenum i_enum);
	void loadBlendFunc();

	static std::hash_map<GLenum, bool> enabledState;

	static GLenum currentCullFace;
	static GLenum currentBlendEquation;
	static GLenum currentBlendFunc[2];
	static std::vector<GLenum> currentDrawBuffers;
}

#endif // OPENGL_HANDLER_H