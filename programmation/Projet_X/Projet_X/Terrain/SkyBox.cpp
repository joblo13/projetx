#include "SkyBox.h"

#include "Terrain.h"
#include "../Math/Matrix.h"
#include "../Camera/Camera.h"
#include "../Shaders/Shader.h"
#include "../Rendering/Texture.h"
#include "../Rendering/DeferredRenderer/DeferredRenderer.h"
#include "../Debugger/GlDebugger.h"



////////////////////////////////////////////////////////////////////////
///
/// @fn SkyBox()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
SkyBox::SkyBox(ShaderType i_shaderType)
:m_angle(0), Drawable(i_shaderType)
{
	load();
}
////////////////////////////////////////////////////////////////////////
///
/// @fn ~SkyBox()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
SkyBox::~SkyBox()
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void void animate(const Uint32 i_timestep)
///
/// Fonction permettant d'animer la skybox
/// 
/// @param[in] i_timestep : différence de temps depuis la dernière animation
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void SkyBox::animate(const Uint32 i_timestep)
{
	m_angle += SKYBOX_ROTATION_SPEED * i_timestep;
	if(m_angle > 360)
		m_angle -= 360;
}

void SkyBox::load()
{
	m_texturesIndex.push_back(Texture::initTexture("TextureSampler", "Texture/skybox_texture.png"));

	glGenVertexArrays(1, &m_VAOID);   
	glBindVertexArray(m_VAOID);


	glGenBuffers(1, &m_buffers[POS_VB]);
	glGenBuffers(1, &m_buffers[INDEX_BUFFER]);

	std::vector<VertexPosition> vertices;
	std::vector<GLuint> indices;

	generateBufferData(vertices, indices);

	Shader::chooseShader(SKYBOX_SHADER);
	Shader* p_currentShader(Shader::getShader());

	Texture::chooseTexture(m_texturesIndex[0]);
	Texture::linkTexture();

	//On ajoute les coordonnées de sommets
	int attribVariable(p_currentShader->getAttribVariableSafe("InVertex"));

  	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), 0);    
	

	//On ajoute les coordonnées de textures
	attribVariable = p_currentShader->getAttribVariableSafe("InUV");

	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(6*sizeof(float) + sizeof(int)));
	
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    //On s'assure que les modifications n'affecte pas l'extérieur
    glBindVertexArray(0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SkyBox::generateBufferData(std::vector<VertexPosition>& i_vertices, std::vector<GLuint>& i_indices)
{
	//Définition des sommets
	VertexPosition vertexInformation;
	//top
	//0
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;

	vertexInformation.u = 1.0f/6.0f;
	vertexInformation.v = 0.0f;

	i_vertices.push_back(vertexInformation);


	//1
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 0.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);

	//2
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;
	
	vertexInformation.u = 0.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//3
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;

	vertexInformation.u = 1.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//bottom
	//4
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;
	
	vertexInformation.u = 1.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);
 
	//5
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;
	
	vertexInformation.u = 2.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);

	//6
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;
	

	vertexInformation.u = 1.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//7
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;
	
	vertexInformation.u = 2.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//front
	//8
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;
	

	vertexInformation.u = 3.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);
 
	//9
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;
	
	vertexInformation.u = 2.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);

	//10
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;
	
	vertexInformation.u = 2.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);
 
	//11
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;
	
	vertexInformation.u = 3.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//back
	//12
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 3.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);
 
	//13
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 4.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);

	//14
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 4.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);
 
	//15
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 3.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//left
	//16
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;
	
	vertexInformation.u = 5.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);

	//17
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 4.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);


	//18
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;
	
	vertexInformation.u = 4.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//19
	vertexInformation.x = SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;

	vertexInformation.u = 5.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//right
	//20
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 1.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);

	//21
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;

	vertexInformation.u = 5.0f/6.0f;
	vertexInformation.v = 0.0f;
	i_vertices.push_back(vertexInformation);
	
	//22
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = -SKYBOX_HALFSIDE;

	vertexInformation.u = 5.0f/6.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//23
	vertexInformation.x = -SKYBOX_HALFSIDE;
	vertexInformation.y = -SKYBOX_HALFSIDE;
	vertexInformation.z = SKYBOX_HALFSIDE;

	vertexInformation.u = 1.0f;
	vertexInformation.v = 1.0f;
	i_vertices.push_back(vertexInformation);

	//Définition de l'index
	//Top face
	i_indices.push_back(0);
	i_indices.push_back(1);
	i_indices.push_back(3);

	i_indices.push_back(1);
	i_indices.push_back(2);
	i_indices.push_back(3);
		
	//Bottom face
	i_indices.push_back(4);
	i_indices.push_back(5);
	i_indices.push_back(7);

	i_indices.push_back(7);
	i_indices.push_back(5);
	i_indices.push_back(6);
		
	//Front face
	i_indices.push_back(8);
	i_indices.push_back(10);
	i_indices.push_back(11);

	i_indices.push_back(8);
	i_indices.push_back(9);
	i_indices.push_back(10);

	//Back face
	i_indices.push_back(12);
	i_indices.push_back(14);
	i_indices.push_back(13);

	i_indices.push_back(12);
	i_indices.push_back(15);
	i_indices.push_back(14);

	//Right face
	i_indices.push_back(16);
	i_indices.push_back(18);
	i_indices.push_back(19);

	i_indices.push_back(16);
	i_indices.push_back(17);
	i_indices.push_back(18);

	//left face
	i_indices.push_back(21);
	i_indices.push_back(23);
	i_indices.push_back(20);

	i_indices.push_back(21);
	i_indices.push_back(22);
	i_indices.push_back(23);

	m_indexSize = i_indices.size();
}

void SkyBox::transformModelview(Matrix& i_modelview, const Camera* ipk_camera)const
{
	Vector positionCamera(ipk_camera->getPosition());
	i_modelview.translate(positionCamera.x, positionCamera.y, positionCamera.z);
	i_modelview.rotate(m_angle, 0, 1, 0);
}

void SkyBox::preTreatementDraw()const
{
	//On change la cullface de côté
	OpenGLHandler::cullFace(GL_FRONT);
}