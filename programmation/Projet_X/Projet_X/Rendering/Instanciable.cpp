#include "Instanciable.h"

#include "../Shaders/Shader.h"
#include "../Math/Matrix.h"
#include "RenderInformation.h"
#include "Texture.h"
#include "../Debugger/GlDebugger.h"

Instanciable::Instanciable(ShaderType i_shaderType)
:Drawable(i_shaderType)
{
	m_normalisationMatrix.loadIdentity();
}

Instanciable::~Instanciable()
{
	
}

void Instanciable::updateInstances()const
{
	getGlError();
	Shader* p_currentShader(Shader::getShader());

	float* matrixValue;

	Matrix matrix;

	int locationModelview(p_currentShader->getAttribVariable("InModelview"));
	if(locationModelview != -1)
	{
		//On remplis les tableaux
		int attribVariable[4];

		//On ajoute les coordonnées de sommets
		attribVariable[0] = locationModelview;
		attribVariable[1] = attribVariable[0] + 1;
		attribVariable[2] = attribVariable[0] + 2;
		attribVariable[3] = attribVariable[0] + 3;

		std::vector<GLfloat> modelviews;
	
		for(int i(0), unitListSize(mp_renderInfoList.size()); i < unitListSize; ++i)
		{
			matrixValue = (m_normalisationMatrix * getModelview(i).transpose()).getValues();
			for(int j(0); j < 16; ++j)
				modelviews.push_back(matrixValue[j]);
		}
		getGlError();
  		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[MODELVIEW_VB]);
		getGlError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(modelviews[0]) * modelviews.size(), &modelviews[0], GL_STREAM_DRAW);
		getGlError();
		for(int i(0); i < 4; ++i)
		{
			glEnableVertexAttribArray(attribVariable[i]);
			glVertexAttribPointer(attribVariable[i], 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(GLfloat) * 4 * i));   
			glVertexAttribDivisor(attribVariable[i], 1);
		}
		
	}
	getGlError();
	updateAdditionnalBuffers();
	getGlError();
}
void Instanciable::initDrawingState()const
{
	Shader* p_currentShader(Shader::getShader());

	//On active les attributs opengl
	int attrib(p_currentShader->getAttribVariable("InVertex"));
	if(attrib != -1)
		glEnableVertexAttribArray(attrib);
	if(m_shaderType == GEOM_PASS_SHADER)
	{
		glEnableVertexAttribArray(p_currentShader->getAttribVariableSafe("InUV"));
		glEnableVertexAttribArray(p_currentShader->getAttribVariableSafe("InModelview"));
		glUniform1i(p_currentShader->getUniformVariableSafe("InExecutionType"), STATIC_MESH);
	}

	glBindVertexArray(m_VAOID);
}
void Instanciable::clearDrawingState()const
{
	glBindVertexArray(0);
	Shader* p_currentShader(Shader::getShader());
	int attrib(p_currentShader->getAttribVariable("InVertex"));
	if(attrib != -1)
		glDisableVertexAttribArray(attrib);
	if(m_shaderType == GEOM_PASS_SHADER)
	{
		glDisableVertexAttribArray(p_currentShader->getAttribVariableSafe("InUV"));
		glDisableVertexAttribArray(p_currentShader->getAttribVariableSafe("InModelview"));
	}
}
void Instanciable::draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const
{
	getGlError();
	if(mp_renderInfoList.size() == 0)
		return;
	Shader::chooseShader(m_shaderType);
	initDrawingState();
	preTreatementDraw();
	Shader* p_shader(Shader::getShader());

	//On calcule la matrice projection * modelview
	Matrix projectionModelview(ik_projection);
	projectionModelview = projectionModelview * i_modelview;

	//On envoit la matrice au shader
	glUniformMatrix4fv(p_shader->getUniformVariableSafe("InProjectionModelview"), 1, GL_TRUE, projectionModelview.getValues());

	if(m_shaderType == GEOM_PASS_SHADER)
	{
		glUniform1f(p_shader->getUniformVariableSafe("InMatSpecularIntensity"), 1.0f);
	}
	getGlError();
	updateInstances();
	getGlError();
	//on affiche chaque entrée du mesh

    for (unsigned int i(0), entriesSize(m_entries.size()); i < entriesSize; ++i) 
	{
        const unsigned int materialIndex = m_entries[i].materialIndex;

        assert(materialIndex < m_texturesIndex.size() || m_shaderType != GEOM_PASS_SHADER);
        
		if(m_shaderType == GEOM_PASS_SHADER)
		{
			if(m_texturesIndex[materialIndex] != -1)
			{
				Texture::chooseTexture(m_texturesIndex[materialIndex]);
				Texture::bindTexture();
				Texture::linkTexture();

				glUniform1f(p_shader->getUniformVariableSafe("InSpecularPower"), m_shininess[materialIndex]);
			}
		}
		getGlError();
		glDrawElementsInstancedBaseVertex(m_drawingMode, m_entries[i].numIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_entries[i].baseIndex), mp_renderInfoList.size(), m_entries[i].baseVertex);
		getGlError();
	}
	postTreatementDraw();
	clearDrawingState();
}

void Instanciable::addToRenderInfoList(RenderInformation& i_renderInformation)
{
	if(i_renderInformation.m_printID == -1)
	{
		mp_renderInfoList.push_back(&i_renderInformation);
		i_renderInformation.m_printID = mp_renderInfoList.size() - 1;
	}
}

void Instanciable::removeFromRenderInfoList(RenderInformation& i_renderInformation)
{
	if(i_renderInformation.m_printID == -1)
		return;
	if(i_renderInformation.m_printID != mp_renderInfoList.size() - 1)
	{
		mp_renderInfoList[i_renderInformation.m_printID] = mp_renderInfoList[mp_renderInfoList.size() - 1];
		mp_renderInfoList[i_renderInformation.m_printID]->m_printID = i_renderInformation.m_printID;
	}
	mp_renderInfoList.pop_back();
	i_renderInformation.m_printID = -1;
}
#include "../Building/TemplateManager.h"
void Instanciable::clearRenderInfoList()
{
	for(int i(0), renderInfoListSize(mp_renderInfoList.size()); i < renderInfoListSize; ++i)
		mp_renderInfoList[i]->m_printID = -1;

	std::vector<RenderInformation*> p_emptyRenderInfoList;
	mp_renderInfoList.swap(p_emptyRenderInfoList);		
}

Matrix Instanciable::getModelview(int i_index)const
{
	return mp_renderInfoList[i_index]->getModelview();
}