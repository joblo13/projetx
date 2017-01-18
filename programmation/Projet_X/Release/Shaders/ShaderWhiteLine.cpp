#include "ShaderWhiteLine.h"

//Variable shader
//enum{IN_VERTEX};
//Variable uniform shader
//enum{PROJECTION_MODELVIEW};

////////////////////////////////////////////////////////////////////////
///
/// @fn ShaderWhiteLine(std::string istr_vertexSource, std::string istr_fragmentSource) 
///
/// Constructeur
/// 
/// @param[in] istr_vertexSource: Chemin d'accès vers la source du vertex Shader
/// @param[in] istr_fragmentSource: Chemin d'accès vers la source du Frament Shader
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
ShaderWhiteLine::ShaderWhiteLine() 
{
	m_vertexSource = "Shaders/glsl/whiteLine.vert";
	m_fragmentSource = "Shaders/glsl/whiteLine.frag";
	m_vertexID = 0;
	m_fragmentID = 0;
	m_programID = 0;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~ShaderWhiteLine() 
///
/// Destructeur
/// Detruit les programmes sur la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
ShaderWhiteLine::~ShaderWhiteLine()
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
void ShaderWhiteLine::bindAttribLocation()
{
	m_attribVariable["InVertex"] = glGetAttribLocation (m_programID, "InVertex");
	m_attribVariable["InModelview"] = glGetAttribLocation (m_programID, "InModelview");
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
void ShaderWhiteLine::bindUniformLocation()
{
	// Verrouillage des entrées Shader
	m_uniformVariable["InProjectionModelview"] = glGetUniformLocation (m_programID, "InProjectionModelview");
	m_uniformVariable["InExecutionType"] = glGetUniformLocation (m_programID, "InExecutionType");
}