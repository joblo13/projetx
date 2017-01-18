#include "Drawable.h"
#include "../Camera/Camera.h"
#include "Texture.h"
#include "../Math/Matrix.h"

#include "../Debugger/GlDebugger.h"

Drawable::Drawable(ShaderType i_shaderType)
:m_ID(-1), m_VAOID(0), m_shaderType(i_shaderType), m_drawingMode(GL_TRIANGLES)
{
}

Drawable::~Drawable()
{
	
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void draw(Matrix& ik_projection, Matrix& i_modelview, Camera& ik_camera)
///
/// Fonction permettant d'afficher une skybox
/// 
/// @param[in] ik_projection : Matrix de projection
/// @param[in] i_modelview : Matrix de modelview	
/// @param[in] ik_camera : Caméra contenant la position du joueur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Drawable::draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const
{
	Shader::chooseShader(m_shaderType);
	preTreatementDraw();
	//On active le shader de la skybox
	Shader* p_currentShader(Shader::getShader());

	//On modifie la position de la skybox selon la position du joueur
	i_modelview.push();

	transformModelview(i_modelview, ipk_camera);

	Matrix projectionModelview(ik_projection);
	projectionModelview = projectionModelview * i_modelview;

	//On envoit la matrice au shader

	int uniformLocation(p_currentShader->getUniformVariable("InProjectionModelview"));
	if(uniformLocation != -1)
		glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, projectionModelview.getValues());
	
	//On charge la texture
	if(m_texturesIndex.size() > 0)
	{
		Texture::chooseTexture(m_texturesIndex[0]);
		Texture::bindTexture();
	}

	glBindVertexArray(m_VAOID);
	glDrawElements(m_drawingMode, m_indexSize, GL_UNSIGNED_INT, (void*)(0 + NULL));
	glBindVertexArray(0);
	glGetError();
	i_modelview.pop();

	postTreatementDraw();
}

void Drawable::addTexture(const std::string& i_variableName, const std::string& i_path)
{
	m_texturesIndex.pop_back();
	m_texturesIndex.push_back(Texture::initTexture(i_variableName, i_path));
}