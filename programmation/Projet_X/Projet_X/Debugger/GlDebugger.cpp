#include "GlDebugger.h"
#include "../Definition.h"
#include <assert.h>

void getGlError()
{
	#if OPENGL_DEBUG == 1
		GL_INVALID_ENUM;
		GL_INVALID_VALUE;
		GL_INVALID_OPERATION;
		GL_INVALID_FRAMEBUFFER_OPERATION;
		GL_OUT_OF_MEMORY;
		int test(glGetError());
		//assert(test == 0);
	#endif
}