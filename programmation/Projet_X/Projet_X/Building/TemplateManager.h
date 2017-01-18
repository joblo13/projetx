#ifndef TEMPLATE_MANAGER_H
#define TEMPLATE_MANAGER_H

#include "../Math/Vector3I.h"
#include "../Math/Vector.h"
#include "Template.h"
#include "../Rendering/WhitePrismeInstanciable.h"

class Chunk;

class TemplateManager
{
    public:
	//Constructeur
	static TemplateManager* getInstance()
	{
		if(mp_instance == 0)
		{
			mp_instance = new TemplateManager();
		}
		return mp_instance;
	}

	static void releaseInstance()
	{
		delete mp_instance;
		mp_instance = 0;
	}

	int createTemplate(const Chunk* ikp_shape, bool i_useCustomShape = false);
	void removeTemplate(int i_index);

	int getTemplateCount() const{return m_templateList.size();}
	Template* getTemplate(int i_index){return m_templateList[i_index];}

	bool isInCollision(Chunk* ip_currentTemplate);
	int templateInCollision(const Vector& i_sourcePosition, const Vector& i_targetPosition);

	WhitePrismeInstanciable* getWhitePrismeInstanciable(){return &m_whitePrismeInstanciable;}

	Chunk* getCurrentTemplateShape(){return mp_currentTemplateShape;}
	void setCurrentTemplateShape(Chunk* ip_currentTemplateShape){mp_currentTemplateShape = ip_currentTemplateShape;}
	void clearCurrentTemplateShape();

	void moveCurrentTemplateShapeBottomCenter(const Vector3I& i_centerPosition);

	void setIsSelected(int i_index, bool i_isSelected);
	void deselectAll();

	void updateHitpointBar();

	void removeWorkerPosition(Unit* ip_unit);

    private:	

	TemplateManager();
    ~TemplateManager();
	//Instance du singleton
	static TemplateManager* mp_instance;

	std::vector<Template*> m_templateList;
	WhitePrismeInstanciable m_whitePrismeInstanciable;
	HitpointBarMesh* mp_hitpointBarMesh;

	Chunk* mp_currentTemplateShape;
};

#endif //TEMPLATE_MANAGER_H
