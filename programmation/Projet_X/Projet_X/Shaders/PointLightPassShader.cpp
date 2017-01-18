#include "PointLightPassShader.h"


PointLightPassShader::PointLightPassShader()
{   
	m_vertexSource = "Shaders/glsl/pointLightPass.vert";
	m_fragmentSource = "Shaders/glsl/pointLightPass.frag";
	m_vertexID = 0;
	m_fragmentID = 0;
	m_programID = 0;
}
PointLightPassShader::~PointLightPassShader()
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
void PointLightPassShader::bindAttribLocation()
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
void PointLightPassShader::bindUniformLocation()
{
	// Verrouillage des entrées Shader
	m_uniformVariable["InProjectionModelview"] = glGetUniformLocation (m_programID, "InProjectionModelview");

	m_uniformVariable["InPositionMap"] = glGetUniformLocation (m_programID, "InPositionMap");
	m_uniformVariable["InColorMap"] = glGetUniformLocation (m_programID, "InColorMap");
	m_uniformVariable["InNormalMap"] = glGetUniformLocation (m_programID, "InNormalMap");
	m_uniformVariable["InEyeWorldPos"] = glGetUniformLocation (m_programID, "InEyeWorldPos");
	m_uniformVariable["InScreenSize"] = glGetUniformLocation (m_programID, "InScreenSize");

	m_uniformVariable["InPointLight.Position"] = glGetUniformLocation (m_programID, "InPointLight.Position");
	m_uniformVariable["InPointLight.Atten.Constant"] = glGetUniformLocation (m_programID, "InPointLight.Atten.Constant");
	m_uniformVariable["InPointLight.Atten.Linear"] = glGetUniformLocation (m_programID, "InPointLight.Atten.Linear");
	m_uniformVariable["InPointLight.Atten.Exp"] = glGetUniformLocation (m_programID, "InPointLight.Atten.Exp");
	m_uniformVariable["InPointLight.Base.Color"] = glGetUniformLocation (m_programID, "InPointLight.Base.Color");
	m_uniformVariable["InPointLight.Base.AmbientIntensity"] = glGetUniformLocation (m_programID, "InPointLight.Base.AmbientIntensity");
	m_uniformVariable["InPointLight.Base.DiffuseIntensity"] = glGetUniformLocation (m_programID, "InPointLight.Base.DiffuseIntensity");
}