#ifndef SHADER_MAP_H
#define SHADER_MAP_H

//Includes interne
#include "Shader.h"

class ShaderMap : public Shader
{
	public:
		ShaderMap();
	private:
		
		//Fonction permettant de trouver la localisation des variables des shaders
		void bindAttribLocation();
		void bindUniformLocation();

	protected:
		//Singleton
		~ShaderMap();
};


#endif //SHADER_MAP_H
