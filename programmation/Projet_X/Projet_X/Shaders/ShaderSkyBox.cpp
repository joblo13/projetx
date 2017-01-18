#include "ShaderSkyBox.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn ShaderSkyBox() 
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
ShaderSkyBox::ShaderSkyBox()
{
	m_vertexSource = "Shaders/glsl/skybox.vert";
	m_fragmentSource = "Shaders/glsl/skybox.frag";
	m_vertexID = 0;
	m_fragmentID = 0;
	m_programID = 0;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~ShaderSkyBox() 
///
/// Destructeur
/// Detruit les programmes sur la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
ShaderSkyBox::~ShaderSkyBox()
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void bindAttribLocation()
///
/// Fonction permettant de créer le lien entre les variables du shader et le programme
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void ShaderSkyBox::bindAttribLocation()
{
	m_attribVariable["InVertex"] = glGetAttribLocation (m_programID, "InVertex");
	m_attribVariable["InUV"] = glGetAttribLocation (m_programID, "InUV");
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void bindUniformLocation()
///
/// Fonction permettant de créer le lien entre les variables uniforme du shader et le programme
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void ShaderSkyBox::bindUniformLocation()
{
	// Verrouillage des entrées Shader
	m_uniformVariable["InProjectionModelview"] = glGetUniformLocation (m_programID, "InProjectionModelview");
	m_uniformVariable["TextureSampler"] = glGetUniformLocation (m_programID, "TextureSampler");
}