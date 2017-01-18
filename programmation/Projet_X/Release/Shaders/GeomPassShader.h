#ifndef GEOM_PASS_SHADER_H
#define GEOM_PASS_SHADER_H

//Includes interne
#include "Shader.h"

class GeomPassShader : public Shader
{
public:
	GeomPassShader();
private:

	//Fonction permettant de trouver la localisation des variables des shaders
	void bindAttribLocation();
	void bindUniformLocation();

protected:
	//Singleton
	~GeomPassShader();
};


#endif //GEOM_PASS_SHADER_H