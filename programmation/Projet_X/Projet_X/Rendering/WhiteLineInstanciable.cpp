#include "WhiteLineInstanciable.h"
#include "../Shaders/Shader.h"
#include "../Math/Vector.h"
#include "../Math/Matrix.h"

#include "../Debugger/GlDebugger.h"
#include "Mesh/Mesh.h"
#include "RenderInformation.h"

WhiteLineInstanciable::WhiteLineInstanciable()
:Instanciable(WHITE_LINE_SHADER)
{
	Shader::chooseShader(WHITE_LINE_SHADER);
	initBuffer();
	m_drawingMode = GL_LINE_LOOP;
}
WhiteLineInstanciable::~WhiteLineInstanciable()
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
void WhiteLineInstanciable::clear()
{       
    if (m_VAOID != 0) 
	{
        glDeleteVertexArrays(1, &m_VAOID);
        m_VAOID = 0;
    }
}
void WhiteLineInstanciable::initBuffer()
{
	glGenVertexArrays(1, &m_VAOID);
	glGenBuffers(1, &m_buffers[POS_VB]);
	glGenBuffers(1, &m_buffers[INDEX_BUFFER]);
	glGenBuffers(1, &m_buffers[MODELVIEW_VB]);
}


void WhiteLineInstanciable::fillBuffer()
{
	//On remplis les tableaux
	int attribVariable;

	//On crée les buffers pour les sommets
	
	glBindVertexArray(m_VAOID);

	Shader::chooseShader(WHITE_LINE_SHADER);
	Shader* p_currentShader(Shader::getShader());

	std::vector<Vector> vertices;
	std::vector<GLuint> indices;

	generateBuffersData(vertices, indices);
	m_indexSize = indices.size();

	//On ajoute les coordonnées de sommets
	attribVariable = p_currentShader->getAttribVariableSafe("InVertex");
	if(attribVariable != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(attribVariable);
		glVertexAttribPointer(attribVariable, 3, GL_FLOAT, GL_FALSE, 0, 0);    
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//glDisableVertexAttribArray(p_currentShader->getAttribVariableSafe("InVertex"));
	//On s'assure que les modifications n'affecte pas l'extérieur
	glBindVertexArray(0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void WhiteLineInstanciable::preTreatementDraw()const
{
	OpenGLHandler::disable(GL_TEXTURE_2D);
	Shader* p_currentShader(Shader::getShader());
	glUniform1i(p_currentShader->getUniformVariableSafe("InExecutionType"), INSTANCIATE_WHITE_LINE);
}

void WhiteLineInstanciable::postTreatementDraw()const
{
	OpenGLHandler::enable(GL_TEXTURE_2D);
}