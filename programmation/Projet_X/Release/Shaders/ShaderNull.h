#ifndef SHADER_NULL_H
#define SHADER_NULL_H

//Includes interne
#include "Shader.h"

class ShaderNull : public Shader
{
	public:
		ShaderNull();

	private:

		
		//Fonction permettant de trouver la localisation des variables des shaders
		void bindAttribLocation();
		void bindUniformLocation();

	protected:
		//Singleton
		~ShaderNull();
};


#endif //SHADER_NULL_H
