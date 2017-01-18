#include "OpenGLHandler.h"
#include <assert.h>

namespace OpenGLHandler
{
	void enable(GLenum i_enum)
	{
		changeState(i_enum, true);
	}

	void disable(GLenum i_enum)
	{
		changeState(i_enum, false);
	}

	void changeState(GLenum i_enum, bool i_target)
	{
		std::hash_map<GLenum, bool>::iterator it(OpenGLHandler::enabledState.find(i_enum));

		bool isAlreadyLoaded(it != OpenGLHandler::enabledState.end());
		bool isChanging(false);

		if(!isAlreadyLoaded)
		{
			GLboolean openGLValue;
			glGetBooleanv(i_enum, &openGLValue);
			isChanging = openGLValue != i_target;
		}
		else
			isChanging = OpenGLHandler::enabledState[i_enum] != i_target;

		if(isChanging)
		{
			if(i_target)
				glEnable(i_enum);
			else
				glDisable(i_enum);
			OpenGLHandler::enabledState[i_enum] = i_target;
		}
	}

	void loadState(GLenum i_enum)
	{
		GLboolean currentValue;
		glGetBooleanv(i_enum, &currentValue);
		OpenGLHandler::enabledState[i_enum] = currentValue;
	}

	void cullFace(GLenum i_enum)
	{
		if(currentCullFace != i_enum)
		{
			glCullFace(i_enum);
			currentCullFace = i_enum;
		}

		#if OPENGL_DEBUG == 1
			GLint test;
			glGetIntegerv(GL_CULL_FACE_MODE, &test);
			if (test != i_enum)
				assert(test == i_enum);
		#endif
	}

	void blendEquation(GLenum i_enum)
	{
		if(currentBlendEquation != i_enum)
		{
			glBlendEquation(i_enum);
			currentBlendEquation = i_enum;
		}

		#if OPENGL_DEBUG == 1
			GLint test;
			glGetIntegerv(GL_BLEND_EQUATION_RGB, &test);
			if (test != i_enum)
				assert(test == i_enum);
		#endif
	}

	void blendFunc(GLenum i_enum0, GLenum i_enum1)
	{
		if(currentBlendFunc[0] != i_enum0 || currentBlendFunc[1] != i_enum1)
		{
			glBlendFunc(i_enum0, i_enum1);
			currentBlendFunc[0] = i_enum0;
			currentBlendFunc[1] = i_enum1;
		}

		#if OPENGL_DEBUG == 1
			GLint test[2];
			glGetIntegerv(GL_BLEND_SRC_RGB, &test[0]);
			glGetIntegerv(GL_BLEND_DST_RGB, &test[1]);
			if(test[0] != i_enum0 || test[1] != i_enum1)
				assert(test[0] == i_enum0 || test[1] == i_enum1);
		#endif
	}
	void drawBuffer(GLenum i_enum)
	{
		std::vector<GLenum> bufs(1, i_enum);
		drawBuffers(bufs);
	}
	void drawBuffers(const std::vector<GLenum>& i_bufs)
	{
		int oldSize(currentDrawBuffers.size());
		int newSize(i_bufs.size());

		bool isChanging(true);
		if(oldSize == newSize)
		{
			isChanging = false;
			for(int i(0); i < oldSize; ++i)
			{
				if(currentDrawBuffers[i] != i_bufs[i])
					isChanging = true;
			}
		}
		if(isChanging == false)
			return;

		if(newSize == 1)
			glDrawBuffer(i_bufs[0]);
		else
			glDrawBuffers(newSize, &i_bufs[0]);

		currentDrawBuffers.reserve(newSize);
		if(currentDrawBuffers.size() != 0)
			std::copy(i_bufs.begin(), i_bufs.end(), currentDrawBuffers.begin());
	}

	void loadBlendFunc()
	{
		GLint currentValue[2];
		glGetIntegerv(GL_BLEND_SRC_RGB, &currentValue[0]);
		glGetIntegerv(GL_BLEND_DST_RGB, &currentValue[1]);
		currentBlendFunc[0] = currentValue[0];
		currentBlendFunc[1] = currentValue[1];
	}
}