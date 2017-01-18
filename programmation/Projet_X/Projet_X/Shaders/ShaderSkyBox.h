#ifndef SHADER_SKYBOX_H
#define SHADER_SKYBOX_H

//Includes interne
#include "Shader.h"

class ShaderSkyBox : public Shader
{
	public:
		ShaderSkyBox();

	private:

		
		//Fonction permettant de trouver la localisation des variables des shaders
		void bindAttribLocation();
		void bindUniformLocation();

	protected:
		//Singleton
		~ShaderSkyBox();
};


#endif //SHADER_SKYBOX_H
