#ifndef DIR_LIGHT_PASS_SHADER_H
#define DIR_LIGHT_PASS_SHADER_H

//Includes interne
#include "Shader.h"

class DirLightPassShader : public Shader
{
public:
	DirLightPassShader();
private:

	//Fonction permettant de trouver la localisation des variables des shaders
	void bindAttribLocation();
	void bindUniformLocation();

protected:
	//Singleton
	~DirLightPassShader();
};


#endif //DIR_LIGHT_PASS_SHADER_H