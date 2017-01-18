#include "Camera.h"

//Include externe
#include <cmath>
#include <algorithm>

//Include interne
#include "../Terrain/Terrain.h"
#include "../SceneOpenGL.h"
#include "../Input.h"
#include "../Entities/UnitHandler.h"
#include "../Building/Building.h"
#include "../Building/TemplateManager.h"

#include "../Shaders/Shader.h"
#include "../Math/Vector.h"
#include "../Math/Vector4f.h"
#include "../Entities/Unit.h"
#include "../Entities/Wieldable/Weapon.h"
#include "CameraRTS.h"
#include "../GUI/GUIHandler.h"


#define MAX_FALLING_SPEED 10
#define MAX_CLIMBING 0.1f
#define MIN_DISTANCE_FALLING 0.2f

Camera::Camera(const Camera& ipk_camera)
:m_sensitivity(CAMERA_SENSITIVITY), 
m_isSaving(ipk_camera.m_isSaving), m_airMouvement(ipk_camera.m_airMouvement),

m_target(ipk_camera.m_target), m_position(ipk_camera.m_position),
m_phi(ipk_camera.m_phi), m_theta(ipk_camera.m_theta), mp_unit(0),
m_selectorStatePriority(ipk_camera.m_selectorStatePriority), m_lastSelectorState(ipk_camera.m_lastSelectorState),
Drawable(NULL_SHADER)
{
	if(ipk_camera.isAUnitSelected())
		setUnit(*ipk_camera.mp_selectedUnits.begin());
	else
		setUnit(ipk_camera.mp_unit);
	UnitHandler::getInstance()->addCamera(this);
	Input::getInstance()->attachObserver(this);

	init();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Camera()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Camera::Camera()
:m_sensitivity(CAMERA_SENSITIVITY), 
m_isSaving(false), m_airMouvement(),

m_target(), m_position(),
m_phi(0), m_theta(0),
mp_unit(0),
m_selectorStatePriority(NONE_UNIT_SELECTOR_STATE), m_lastSelectorState(INIT_SELECTOR_STATE),
Drawable(NULL_SHADER)
{
	UnitHandler::getInstance()->addCamera(this);
	Input::getInstance()->attachObserver(this);

	init();
}

void Camera::init()
{
	GUIHandler* p_guiHandler(GUIHandler::getInstance());
	p_guiHandler->setVisibility(FPS_LAYOUT, true);
	p_guiHandler->setVisibility(RTS_LAYOUT, false);
	p_guiHandler->setVisibility(MAP_LAYOUT, true);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Camera()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{
	setUnit(0);
	Input::getInstance()->removeObserver(this);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void OnMouseMotion(int i_x, int i_y)
///
/// Évenement lancé lorsque l'on bouge la souris
/// 
/// @param[in] i_x : position de la souris en x
/// @param[in] i_y : position de la souris en y
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Camera::OnMouseMotion(int i_x, int i_y)
{
	Input* p_input(Input::getInstance());
	if(!p_input->getMouseVisible())
	{
		int centerX, centerY;
		SDL_GetWindowSize(*SceneOpenGL::getInstance()->getWindow(), &centerX, &centerY);
		centerX /= 2;
		centerY /= 2;

		//On calcule la position du clavier par rapport au centre de la fenêtre
		float xRel = (float)(centerX - i_x);
		float yRel = (float)(centerY - i_y);

		//On calcule la différence d'angle imposée par la position de la souris
		setTheta(m_theta - xRel * m_sensitivity);
		setPhi(m_phi + yRel * m_sensitivity);

		//On modifie les vecteurs selon les angles
		vectorsFromAngles();

		//On replace le curseur au centre de la fenêtre
	
		p_input->centerMouse();
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void animate(Uint32 i_timestep)
///
/// Effet des différentes évènements et du temp sur la caméra
/// 
/// @param[in] i_timestep : différence de temps depuis la dernière animation
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Camera::animate(Uint32 i_timestep)
{
	m_lastSelectorState = getSelectorState();
	UnitHandler::getInstance()->updateHitpointBar();
	TemplateManager::getInstance()->updateHitpointBar();

	Input* p_input(Input::getInstance());

	if(p_input->getKeyPress("changeView") && p_input->getInputState() != CONSOLE_INPUT)
	{
		p_input->setCameraType(RTS_CAMERA);
	}

	if(!p_input->getMouseVisible())
	{
		mp_unit->setIsShooting(0, p_input->getMouseButState(MOUSE_LEFT), p_input->getMouseButPress(MOUSE_LEFT));
		mp_unit->setIsShooting(2, p_input->getMouseButState(MOUSE_RIGHT), p_input->getMouseButPress(MOUSE_RIGHT));
		mp_unit->setIsReloading(p_input->getKeyState("reload"));

		//On change d'arme si une des touches est appuyé
		if(p_input->getKeyState("switchGun1"))mp_unit->switchWeapon(1);
		if(p_input->getKeyState("switchGun2"))mp_unit->switchWeapon(2);
		if(p_input->getKeyState("switchGun3"))mp_unit->switchWeapon(3);
		if(p_input->getKeyState("switchGun4"))mp_unit->switchWeapon(4);
		if(p_input->getKeyState("switchGun5"))mp_unit->switchWeapon(5);
		if(p_input->getKeyState("switchGun6"))mp_unit->switchWeapon(6);
		if(p_input->getKeyState("switchGun7"))mp_unit->switchWeapon(7);
		if(p_input->getKeyState("switchGun8"))mp_unit->switchWeapon(8);
		if(p_input->getKeyState("switchGun9"))mp_unit->switchWeapon(9);
		if(p_input->getKeyState("switchGun0"))mp_unit->switchWeapon(0);
	}
	vectorsFromAngles();
	Terrain* p_terrain(Terrain::getInstance());

	//On modifie la vitesse lorsque l'on saute
	//Si on ne tombe pas
		if(p_input->getInputState() == FPS_INPUT)
		{
			//On modifie la vitesse si la touche rapide est activé
			bool speedBoost = p_input->getKeyState("boost");

			int frontDir = 0;
			int sideDir = 0;
			//Pour chaque deplacement, effectué on déplace la caméra et conserve en mémoire le déplacement
			if (p_input->getKeyState("forward"))
			{
				frontDir++;
			}
			if (p_input->getKeyState("backward"))
			{
				frontDir--;
			}
			if (p_input->getKeyState("strafe_left"))
			{
				sideDir++;
			}
			if (p_input->getKeyState("strafe_right"))
			{
				sideDir--;
			}
			//Si l'on a appuyé sur la touche du saut, on donne une vitesse vertical à la caméra
			bool isJumping = p_input->getKeyState("jump");

			mp_unit->moveFPS(m_walkingDirection, m_left, sideDir, frontDir, speedBoost, isJumping, i_timestep);

		}

	if(p_input->getKeyState("materialSelection"))
		GUIHandler::getInstance()->setVisibility(MATERIAL_SELECTION_LAYOUT, true);

	if(p_input->getKeyPress("weaponShop") && p_input->getInputState() != CONSOLE_INPUT)
	{
		GUIHandler* p_guiHandler(GUIHandler::getInstance());

		p_guiHandler->toggleVisibility(WEAPONSHOP_LAYOUT);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void vectorsFromAngles()
///
/// Fonction modifiant les vecteurs mx_walkingDirection, _foward, mx_left et mx_target
/// selon les angles actuels
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Camera::vectorsFromAngles(bool i_reversed)
{
    if (m_phi > 89.9f)
        m_phi = 89.9f;
    else if (m_phi < -89.9f)
        m_phi = -89.9f;

    float temp = cos(m_phi*MATH_PI/180);

    m_forward.z = temp*sin(m_theta*MATH_PI/180);
    m_forward.x = temp*cos(m_theta*MATH_PI/180);
    m_forward.y = sin(m_phi*MATH_PI/180);
	m_forward.normalize();

	m_walkingDirection.z = sin(m_theta*MATH_PI/180);
	m_walkingDirection.x = cos(m_theta*MATH_PI/180);
	m_walkingDirection.y = 0;
	m_walkingDirection.normalize();

    setLeftFromFoward();

	if(i_reversed)
		setPositionFromTarget();
	else
		setTargetFromPosition();

}

////////////////////////////////////////////////////////////////////////
///
/// @fn void look(Matrix &i_modelview)
///
/// Fonction permettant de modifier la matrice de modelview selon la
/// position de la caméra
/// 
/// @param[in] i_modelview : matrice gérant les transformations géometriques
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Camera::look(Matrix& i_modelview, Matrix& i_projection, int i_windowWidth, int i_windowHeight)
{
	m_modelview = i_modelview;
	m_projection = i_projection;

	updateCamera();

	//On remet la matrice à zéro
    i_modelview.loadIdentity();
	//Modification de la matrice de modelview
    i_modelview.lookAt(m_position.x, m_position.y, m_position.z,
						m_target.x, m_target.y, m_target.z,
						0,1,0);

	//On modifie la perspective selon le zoom de l'arme
	i_projection.loadIdentity();
	i_projection.perspective(mp_unit?mp_unit->getViewAngle():DEFAULT_VIEW_ANGLE, (float) i_windowWidth / i_windowHeight, RENDERING_DISTANCE_NEAR, RENDERING_DISTANCE_FAR);	
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void update()
///
/// Fonction dérivant de Observer permettant de réagir au mouvement de la souris
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Camera::update()
{
	Input* p_input(Input::getInstance());
	int n_x = p_input->getMousePosX();
	int n_y = p_input->getMousePosY();
	OnMouseMotion(n_x, n_y);
}

void Camera::updateCamera()
{
	m_theta = mp_unit->m_theta;
	m_phi = mp_unit->m_phi;
	m_position = mp_unit->getPosition();
	m_position.y += mp_unit->m_hitBox.getDimension(HEIGHT);

	vectorsFromAngles();
}

void Camera::setPosition(const Vector& ik_position)
{
	m_position = ik_position;
	if(mp_unit != 0)
	{
		mp_unit->setPosition(ik_position);
		m_position.y += mp_unit->m_hitBox.getDimension(HEIGHT);
	}
	
}
void Camera::setPositionFromTarget()
{
	setPosition(m_target - 5 * m_forward);
}

void Camera::setTarget(const Vector& ik_target)
{
	if(mp_unit != 0)
		mp_unit->setTarget(ik_target);
	m_target = ik_target;
}
void Camera::setTargetFromPosition()
{
	Vector target(m_position + m_forward);
	if(mp_unit != 0)
		mp_unit->setTarget(target);
	m_target = target;
}

void Camera::setPhi(float i_phi)
{
	if(mp_unit != 0)
		mp_unit->m_phi = i_phi;
	m_phi = i_phi;
}

void Camera::setTheta(float i_theta)
{
	if(i_theta < 0.0f)
		i_theta += 360.0f * ceil(-(i_theta/360.0f));

	if(i_theta > 360.0f)
		i_theta -= 360.0f * ceil((i_theta/360.0f));

	if(mp_unit != 0)
		mp_unit->m_theta = i_theta;
	m_theta = i_theta;
}

Vector Camera::getPosition()const
{
	return m_position;
}

float Camera::getPhi()const
{
	return m_phi;
}
float Camera::getTheta()const
{
	return m_theta;
}
bool Camera::isUnitUsed(Unit* ip_unit)
{
	return ip_unit == mp_unit;
}

bool Camera::isUnitSelected(Unit* ip_unit)const
{
	return mp_selectedUnits.find(ip_unit) != mp_selectedUnits.end();
}

bool Camera::isAUnitSelected() const
{
	return !mp_selectedUnits.empty();
}

int Camera::getUnitID()const
{
	if(mp_unit == 0)
		return -1;
	else
		return mp_unit->m_unitID;
}

Vector Camera::getTarget()const
{
	return m_target;
}

std::hash_set<Unit*> Camera::getSelectedUnit()const
{
	return mp_selectedUnits;
}

float roundf(float i_x)
{
   return (i_x - (int)i_x < 0.5f)?floor(i_x):ceil(i_x);
}

void Camera::getScreenPosition(const Vector& i_position, int& o_x, int& o_y) 
{
	Matrix viewProjectionMatrix = m_projection * m_modelview;
	//transform world to clipping coordinates
	Vector4f position(i_position, 1.f);
	position = viewProjectionMatrix * position;
	position /= position.w;

	int windowWidth, windowHeight;
	SDL_GetWindowSize(*SceneOpenGL::getInstance()->getWindow(), &windowWidth, &windowHeight);

	o_x = (int) roundf((( position.x + 1 ) / 2.0) * windowWidth );
	o_y = (int) roundf((( 1 - position.y ) / 2.0) * windowHeight );
}

void Camera::setUnit(Unit* ip_unit)
{
	if(mp_unit != 0)
	{
		Weapon* p_weapon(mp_unit->getCurrentWeapon());
		if(p_weapon != 0)
			p_weapon->removeElement();
		mp_unit->addElement();
		
	}

	if(ip_unit != 0)
	{
		ip_unit->removeElement();
		Weapon* p_weapon(ip_unit->getCurrentWeapon());
		if(p_weapon != 0)
			p_weapon->addElement();
	}
	mp_unit = ip_unit;
}

Vector Camera::getLeft()const
{
	return m_left;
}

void Camera::setLeftFromFoward()
{
	static const Vector k_up(0,1,0);

	if(mp_unit != 0)
		m_left = *mp_unit->setLeft();
	else
	{
		m_left = k_up.crossProduct(m_forward);
		m_left.normalize();
	}

}

SelectorState Camera::getSelectorState()const
{
	bool isUnitSelected(!mp_selectedUnits.empty());
	bool isTemplateSelected(!mp_selectedTemplates.empty());

	if(isUnitSelected && !isTemplateSelected)
		return UNIT_SELECTOR_STATE;
	if(!isUnitSelected && isTemplateSelected)
		return BULDING_SELECTOR_STATE;
	if(!isUnitSelected && !isTemplateSelected)
		return NONE_UNIT_SELECTOR_STATE;
	if(isUnitSelected && isTemplateSelected)
		return m_selectorStatePriority;
}

bool Camera::isSelectorStateChanged()const
{
	return m_lastSelectorState != getSelectorState();
}

void Camera::upgradeTemplates()
{
	
		for(std::hash_set<Template*>::iterator it(mp_selectedTemplates.begin()), selectedTemplatesEnd(mp_selectedTemplates.end()); it != selectedTemplatesEnd; ++it)
			(*it)->upgrade();
}