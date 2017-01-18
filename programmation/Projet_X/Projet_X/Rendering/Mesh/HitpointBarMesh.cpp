#include "HitpointBarMesh.h"
#include "../../Shaders/Shader.h"
#include "../../Math/Vector.h"
#include "../../Math/Matrix.h"
#include "../DeferredRenderer/DeferredRenderer.h"

#include "../../Debugger/GlDebugger.h"
#include "Mesh.h"
#include "../RenderInformation.h"
#include"../../Entities/Unit.h"

HitpointBarMesh::HitpointBarMesh()
:Mesh("../../../graphique/mesh/quad.obj", GEOM_PASS_SHADER, BLUE_ALPHA | SKIP_LIGHT)
{
	if(!loadMesh(m_path))
		exit(-10);

	if(m_shaderType == GEOM_PASS_SHADER)
		DeferredRenderer::getInstance()->addDrawable(this);

	Shader::chooseShader(GEOM_PASS_SHADER);
}
HitpointBarMesh::~HitpointBarMesh()
{
	clear();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void clear()
///
/// Fonction permettant de vider la mémoire de la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void HitpointBarMesh::clear()
{       
}

void HitpointBarMesh::preTreatementDraw()const
{
	
	if(m_shaderType == GEOM_PASS_SHADER)
	{
		Shader* p_currentShader(Shader::getShader());
		glEnableVertexAttribArray(p_currentShader->getAttribVariableSafe("InHealthPercent"));
		glEnableVertexAttribArray(p_currentShader->getAttribVariableSafe("InArmorPercent"));
		glUniform1i(p_currentShader->getUniformVariableSafe("InExecutionType"), HITPOINT_BAR);
	}
	else
		assert(false);
	Mesh::preTreatementDraw();
}

void HitpointBarMesh::postTreatementDraw()const
{
	Mesh::postTreatementDraw();
	if(m_shaderType == GEOM_PASS_SHADER)
	{
		Shader* p_currentShader(Shader::getShader());
		glDisableVertexAttribArray(p_currentShader->getAttribVariableSafe("InHealthPercent"));
		glDisableVertexAttribArray(p_currentShader->getAttribVariableSafe("InArmorPercent"));
	}
}

void HitpointBarMesh::updateAdditionnalBuffers()const
{
	Shader* p_currentShader(Shader::getShader());
	int locationHealthPercent(p_currentShader->getAttribVariable("InHealthPercent"));
	if(locationHealthPercent != -1)
	{
		std::vector<GLfloat> healthPercent;
	
		HitpointEntity* p_hitpointEntry(0);
		for(int i(0), unitListSize(mp_renderInfoList.size()); i < unitListSize; ++i)
		{
			p_hitpointEntry = (HitpointEntity *)mp_renderInfoList[i]->getObject();
			healthPercent.push_back(p_hitpointEntry->getHealthPercent());
		}

		getGlError();
  		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[HITPOINT_BAR_VB]);
		getGlError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(healthPercent[0]) * healthPercent.size(), &healthPercent[0], GL_STREAM_DRAW);
		getGlError();
		glEnableVertexAttribArray(locationHealthPercent);
		glVertexAttribPointer(locationHealthPercent, 1, GL_FLOAT, GL_FALSE, 0, (void*)(0));   
		glVertexAttribDivisor(locationHealthPercent, 1);
		
	}

	int locationArmorPercent(p_currentShader->getAttribVariable("InArmorPercent"));
	if(locationArmorPercent != -1)
	{
		std::vector<GLfloat> armorPercent;
	
		HitpointEntity* p_hitpointEntry(0);
		for(int i(0), unitListSize(mp_renderInfoList.size()); i < unitListSize; ++i)
		{
			p_hitpointEntry = (HitpointEntity *)mp_renderInfoList[i]->getObject();
			armorPercent.push_back(p_hitpointEntry->getArmorPercent());
		}

		getGlError();
  		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[ARMOR_BAR_VB]);
		getGlError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(armorPercent[0]) * armorPercent.size(), &armorPercent[0], GL_STREAM_DRAW);
		getGlError();
		glEnableVertexAttribArray(locationArmorPercent);
		glVertexAttribPointer(locationArmorPercent, 1, GL_FLOAT, GL_FALSE, 0, (void*)(0));   
		glVertexAttribDivisor(locationArmorPercent, 1);
		
	}
}