#ifndef CAMERA_H
#define CAMERA_H

//Include externe
#include <SDL/SDL.h>
#include <string>
#include <hash_set>

//Include interne
#include "../Definition.h"
#include "../Observer.h"
#include "../HitBox.h"
#include "../Rendering/Drawable.h"
#include "../Math/Matrix.h"
#include "../Building/Template.h"

enum CameraType{FPS_CAMERA, RTS_CAMERA};
enum SelectorState{INIT_SELECTOR_STATE, NONE_UNIT_SELECTOR_STATE, UNIT_SELECTOR_STATE, BULDING_SELECTOR_STATE};
#define CAMERA_SENSITIVITY 0.2f

class Template;
class Vector;
class Unit;

class Camera:public Observer, public Drawable
{
public:
	Camera();
	Camera(const Camera& ipk_camera);
	virtual ~Camera();

    //Fonction d'affichage
    virtual void animate(Uint32 i_timestep);
	virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const{}
    virtual void look(Matrix& i_modelview, Matrix& i_projection, int i_windowWidth, int i_windowHeight);

	virtual CameraType getCameraType(){return FPS_CAMERA;}

    //Fonction de modification des attributs
    void setSensitivity(float i_sensitivity){m_sensitivity = i_sensitivity;}

	void setUnit(Unit* ip_unit);

	//Fonction de recupération des attributs
	float getPhi()const;
	float getTheta()const;
	Vector getLeft()const;
	Vector getPosition()const;
	Vector getTarget()const;
	Vector getForward()const{return m_forward;}
	Vector getWalkingDirection()const{return m_walkingDirection;}
	bool isUnitUsed(Unit* ip_unit);

	int getUnitID()const;

	std::hash_set<Unit*> getSelectedUnit()const;

	void getScreenPosition(const Vector& i_position, int& o_x, int& o_y);


	//Gestion d'évenement
	virtual void update();

	bool isUnitSelected(Unit* ip_unit) const;

	bool isAUnitSelected() const;

	SelectorState getSelectorState()const;
	bool isSelectorStateChanged()const;

	void upgradeTemplates();

	void init();

protected:	
	//Action
	void damageCube();

	//Fonction convertissant les angles en un vecteur unitaire
    void vectorsFromAngles(bool i_reversed = false);

	//Fonction gérant les événements
    virtual void OnMouseMotion(int in_x, int in_y);

	virtual void updateCamera();

    void setPosition(const Vector& ik_position);
	void setPositionFromTarget();
	void setTarget(const Vector& ik_target);
	void setTargetFromPosition();
	void setPhi(float i_phi);
	void setTheta(float i_theta);

	void setLeftFromFoward();

    //Setting
    float m_sensitivity;
	
	//État
	bool m_isSaving;
    //Positionnement de la camera
    Vector m_forward;
	Vector m_walkingDirection;

	Vector m_target;
	Vector m_position;
	Vector m_left;

	float m_phi;
	float m_theta;

	Vector m_airMouvement;
	Unit* mp_unit;

	std::hash_set<Unit*> mp_selectedUnits;
	std::hash_set<Template*> mp_selectedTemplates;

	SelectorState m_selectorStatePriority;
	SelectorState m_lastSelectorState;

	Matrix m_modelview;
	Matrix m_projection;
};

#endif //CAMERA_H
