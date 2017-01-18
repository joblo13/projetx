#include "GeomPassShader.h"

#include "../Debugger/GlDebugger.h"

GeomPassShader::GeomPassShader()
{   
	m_vertexSource = "Shaders/glsl/geometryPass.vert";
	m_fragmentSource = "Shaders/glsl/geometryPass.frag";
	m_vertexID = 0;
	m_fragmentID = 0;
	m_programID = 0;
}

GeomPassShader::~GeomPassShader()
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
void GeomPassShader::bindAttribLocation()
{
	m_attribVariable["InVertex"] = glGetAttribLocation (m_programID, "InVertex");
	m_attribVariable["InUV"] = glGetAttribLocation (m_programID, "InUV");
	m_attribVariable["InNormal"] = glGetAttribLocation (m_programID, "InNormal");
	//Terrain
	m_attribVariable["InColor"] = glGetAttribLocation (m_programID, "InColor");
	m_attribVariable["InTextureNb"] = glGetAttribLocation (m_programID, "InTextureNb");
	m_attribVariable["InDamage"] = glGetAttribLocation (m_programID, "InDamage");
	m_attribVariable["InRenforcement"] = glGetAttribLocation (m_programID, "InRenforcement");
	//Animated mesh
	m_attribVariable["InBoneIDs"] = glGetAttribLocation (m_programID, "InBoneIDs");
	m_attribVariable["InWeights"] = glGetAttribLocation (m_programID, "InWeights");
	m_attribVariable["InModelview"] = glGetAttribLocation (m_programID, "InModelview");
	m_attribVariable["InHealthPercent"] = glGetAttribLocation (m_programID, "InHealthPercent");
	m_attribVariable["InArmorPercent"] = glGetAttribLocation (m_programID, "InArmorPercent");
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
void GeomPassShader::bindUniformLocation()
{
	// Verrouillage des entrées Shader
	m_uniformVariable["InProjectionModelview"] = glGetUniformLocation (m_programID, "InProjectionModelview");
	m_uniformVariable["InWorld"] = glGetUniformLocation (m_programID, "InWorld");
	m_uniformVariable["TextureSampler"] = glGetUniformLocation (m_programID, "TextureSampler");
	m_uniformVariable["NbTextureTot"] = glGetUniformLocation (m_programID, "NbTextureTot");
	m_uniformVariable["InExecutionType"] = glGetUniformLocation (m_programID, "InExecutionType");
	m_uniformVariable["InMatSpecularIntensity"] = glGetUniformLocation (m_programID, "InMatSpecularIntensity");
	m_uniformVariable["InSpecularPower"] = glGetUniformLocation (m_programID, "InSpecularPower");

	m_uniformVariable["AnimMatrices"] = glGetUniformLocation (m_programID, "AnimMatrices");
	m_uniformVariable["numBones"] = glGetUniformLocation (m_programID, "numBones");
}