#ifndef POINT_LIGHT_PASS_SHADER_H
#define POINT_LIGHT_PASS_SHADER_H

//Includes interne
#include "Shader.h"

class PointLightPassShader : public Shader
{
public:
	PointLightPassShader();
private:

	//Fonction permettant de trouver la localisation des variables des shaders
	void bindAttribLocation();
	void bindUniformLocation();

protected:
	//Singleton
	~PointLightPassShader();
};


#endif //POINT_LIGHT_PASS_SHADER_H