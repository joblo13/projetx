#ifndef SHADER_WHITE_LINE_H
#define SHADER_WHITE_LINE_H

//Includes interne
#include "Shader.h"

class ShaderWhiteLine : public Shader
{
	public:
		ShaderWhiteLine();

	private:

		
		//Fonction permettant de trouver la localisation des variables des shaders
		void bindAttribLocation();
		void bindUniformLocation();

	protected:
		//Singleton
		~ShaderWhiteLine();
};


#endif //SHADER_WHITE_LINE_H
