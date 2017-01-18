#include "TemplateManager.h"

#include "../Terrain/Terrain.h"
#include "../Rendering/DeferredRenderer/DeferredRenderer.h"
#include "../Terrain/Chunk.h"
#include "../HitBox.h"
#include "../SceneOpenGL.h"
#include "../Camera/Camera.h"

TemplateManager* TemplateManager::mp_instance(0);

TemplateManager::TemplateManager()
:mp_currentTemplateShape(0)
{
	mp_hitpointBarMesh = SceneOpenGL::getInstance()->getHitpointBarMesh();
	DeferredRenderer::getInstance()->addWhiteLine((Instanciable*)(&m_whitePrismeInstanciable));
}
TemplateManager::~TemplateManager()
{
	for(int i(0), templateListSize(m_templateList.size()); i< templateListSize; ++i)
		delete m_templateList[i];
}

int TemplateManager::createTemplate(const Chunk* ikp_shape, bool i_useCustomShape)
{
	m_templateList.push_back(new Template(ikp_shape, i_useCustomShape, &m_whitePrismeInstanciable, mp_hitpointBarMesh));
	
	//On modifie la map
	Terrain* p_terrain(Terrain::getInstance());
	unsigned char color[3] = {255, 0, 0};

	for(int i(m_templateList[m_templateList.size()-1]->getCubePosition().x), 
		xSize(m_templateList[m_templateList.size()-1]->getCubePosition().x + m_templateList[m_templateList.size()-1]->getCubeDimension(WIDTH)); i < xSize; ++i)
		for(int j(m_templateList[m_templateList.size()-1]->getCubePosition().z), 
			ySize(m_templateList[m_templateList.size()-1]->getCubePosition().z + m_templateList[m_templateList.size()-1]->getCubeDimension(LENGTH)); j < ySize; ++j)
			p_terrain->setMapOverviewSingleColor(i, j, color);

	return m_templateList.size() - 1;
}
void TemplateManager::removeTemplate(int i_index)
{
	//On modifie la map
	int x[2] = {m_templateList[m_templateList.size()-1]->getCubePosition().x, m_templateList[m_templateList.size()-1]->getCubePosition().x + m_templateList[m_templateList.size()-1]->getCubeDimension(WIDTH)};
	int z[2] = {m_templateList[m_templateList.size()-1]->getCubePosition().z, m_templateList[m_templateList.size()-1]->getCubePosition().z + m_templateList[m_templateList.size()-1]->getCubeDimension(LENGTH)};

	delete m_templateList[i_index];
	m_templateList[i_index] = m_templateList[m_templateList.size()-1];
	m_templateList.pop_back();

	Terrain* p_terrain(Terrain::getInstance());
	for(int i(x[0]); i < x[1]; ++i)
		for(int j(z[0]); j < z[1]; ++j)
			p_terrain->reloadMapOverviewSingleColor(i, j);
}

bool TemplateManager::isInCollision(Chunk* ip_currentTemplate)
{
	Vector3I templatePosition;
	Vector3I templateSize;
	Vector3I currentTemplatePosition(ip_currentTemplate->getCubePosition());
	Vector3I currentTemplateSize(ip_currentTemplate->getSizeX(), ip_currentTemplate->getSizeY(), ip_currentTemplate->getSizeZ());
	
	for(int i(0), templateListSize(m_templateList.size()); i < templateListSize; ++i)
	{
		templatePosition = m_templateList[i]->getCubePosition();
		templateSize.x = m_templateList[i]->getDimension(WIDTH);
		templateSize.y = m_templateList[i]->getDimension(HEIGHT);
		templateSize.z = m_templateList[i]->getDimension(LENGTH);

		if(!(templatePosition.x > currentTemplatePosition.x + currentTemplateSize.x || 
			templatePosition.x + templateSize.x < currentTemplatePosition.x || 
			templatePosition.y > currentTemplatePosition.y + currentTemplateSize.y || 
			templatePosition.y + templateSize.y < currentTemplatePosition.y || 
			templatePosition.z > currentTemplatePosition.z + currentTemplateSize.z || 
			templatePosition.z + templateSize.z < currentTemplatePosition.z))
			return true;
	}
	return false;
}

int TemplateManager::templateInCollision(const Vector& i_sourcePosition, const Vector& i_targetPosition)
{
	Vector templatePosition;
	HitBox hitBox;

	for(int i(0), templateListSize(m_templateList.size()); i < templateListSize; ++i)
	{
		hitBox = m_templateList[i]->getHitBox();

		if(hitBox.rayCollision(i_sourcePosition, i_targetPosition))
				return i;
	}
	return -1;
}

void TemplateManager::clearCurrentTemplateShape()
{
		DeferredRenderer::getInstance()->removeDrawable(mp_currentTemplateShape);
		mp_currentTemplateShape = 0;
}

void TemplateManager::moveCurrentTemplateShapeBottomCenter(const Vector3I& i_centerPosition)
{
	mp_currentTemplateShape->moveChunkBottomCenter(i_centerPosition);
}

void TemplateManager::setIsSelected(int i_index, bool i_isSelected)
{
	m_templateList[i_index]->setIsSelected(i_isSelected);
}

void TemplateManager::deselectAll()
{
	for(int i(0),teamCount(m_templateList.size()); i < teamCount; ++i)
	{
		m_templateList[i]->setIsSelected(false);
	}
	if(mp_currentTemplateShape != 0)
		mp_currentTemplateShape->removeWhitePrismeDrawable();
}

void TemplateManager::updateHitpointBar()
{
	Vector cameraForward(SceneOpenGL::getInstance()->getCamera()->getForward());

	//On itère pour tous les unités
	for(int i(0), templateListSize(m_templateList.size()); i < templateListSize; ++i)
	{
		if(m_templateList[i]->updateHitpointBar(cameraForward))
		{
			removeTemplate(i);
			//on doit repasser sur la nouvelle partie
			--i;
			--templateListSize;
		}
	}

}

void TemplateManager::removeWorkerPosition(Unit* ip_unit)
{
	for(int i(0), templateListSize(m_templateList.size()); i < templateListSize; ++i)
	{
		m_templateList[i]->removeWorkerPosition(ip_unit);
	}
}