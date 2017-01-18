#include "DirLightPassShader.h"

DirLightPassShader::DirLightPassShader()
{   
	m_vertexSource = "Shaders/glsl/dirLightPass.vert";
	m_fragmentSource = "Shaders/glsl/dirLightPass.frag";
	m_vertexID = 0;
	m_fragmentID = 0;
	m_programID = 0;
}

DirLightPassShader::~DirLightPassShader()
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
void DirLightPassShader::bindAttribLocation()
{
	m_attribVariable["InVertex"] = glGetAttribLocation (m_programID, "InVertex");
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
void DirLightPassShader::bindUniformLocation()
{
		// Verrouillage des entrées Shader
		m_uniformVariable["InProjectionModelview"] = glGetUniformLocation (m_programID, "InProjectionModelview");

		m_uniformVariable["InPositionMap"] = glGetUniformLocation (m_programID, "InPositionMap");
		m_uniformVariable["InColorMap"] = glGetUniformLocation (m_programID, "InColorMap");
		m_uniformVariable["InNormalMap"] = glGetUniformLocation (m_programID, "InNormalMap");
		m_uniformVariable["InEyeWorldPos"] = glGetUniformLocation (m_programID, "InEyeWorldPos");
		m_uniformVariable["InScreenSize"] = glGetUniformLocation (m_programID, "InScreenSize");

		m_uniformVariable["InDirectionalLight.Direction"] = glGetUniformLocation (m_programID, "InDirectionalLight.Direction");
		m_uniformVariable["InDirectionalLight.Base.Color"] = glGetUniformLocation (m_programID, "InDirectionalLight.Base.Color");
		m_uniformVariable["InDirectionalLight.Base.AmbientIntensity"] = glGetUniformLocation (m_programID, "InDirectionalLight.Base.AmbientIntensity");
		m_uniformVariable["InDirectionalLight.Base.DiffuseIntensity"] = glGetUniformLocation (m_programID, "InDirectionalLight.Base.DiffuseIntensity");
}