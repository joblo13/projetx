#include "ShaderMap.h"

//Variable shader
//enum{IN_VERTEX, IN_UV};
//Variable uniform shader
//enum{PROJECTION_MODELVIEW, TEXTURE_SAMPLER};

////////////////////////////////////////////////////////////////////////
///
/// @fn ShaderMap() 
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
ShaderMap::ShaderMap() 
{
	m_vertexSource = "Shaders/glsl/map.vert";
	m_fragmentSource = "Shaders/glsl/map.frag";
	m_vertexID = 0;
	m_fragmentID = 0;
	m_programID = 0;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~ShaderMap() 
///
/// Destructeur
/// Detruit les programmes sur la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
ShaderMap::~ShaderMap()
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
void ShaderMap::bindAttribLocation()
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
void ShaderMap::bindUniformLocation()
{
	// Verrouillage des entrées Shader
	m_uniformVariable["InUVMatrix"] = glGetUniformLocation (m_programID, "InUVMatrix");
	m_uniformVariable["MapTexture"] = glGetUniformLocation (m_programID, "MapTexture");
}