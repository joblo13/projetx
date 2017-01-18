#ifndef CAMERA_RTS_H
#define CAMERA_RTS_H

//Include externe
#include <SDL/SDL.h>
#include <map>
#include <string>

//Include interne
#include "../Definition.h"
#include "../Observer.h"
#include "../HitBox.h"
#include "Camera.h"
#include "../Rendering/WhiteRectangle.h"
#include "../Terrain/Chunk.h"

class Vector;
class Matrix;
class Unit;

enum CameraState{DEFAULT_STATE, 
	ZONE_SELECTING0_TEMPLATE_STATE, ZONE_SELECTING1_TEMPLATE_STATE, ZONE_SELECTING2_TEMPLATE_STATE, 
	ZONE_SELECTING0_FILLER_STATE, ZONE_SELECTING1_FILLER_STATE, ZONE_SELECTING2_FILLER_STATE, 
	PRE_TEMPLATE_SELECTED_STATE, TEMPLATE_SELECTED_STATE, CONSOLE_STATE};

class CameraRTS : public Camera
{
public:
	CameraRTS(const Camera& ipk_camera);
    CameraRTS();
	virtual ~CameraRTS();

	void init();

    //Fonction d'affichage
    virtual void animate(Uint32 i_timestep);
	virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const;
	
	virtual CameraType getCameraType(){return RTS_CAMERA;}

	virtual void update();

	void selectCurrentTemplateShape(int i_index);

	void switchToTemplateSelectionMode();
	void switchToFillSelectionMode();

	void unitMoving(const Vector3I& i_cubeTarget, int i_templateID);

protected:	
	//Fonction gérant les événements
	virtual void updateCamera(){}
    virtual void OnMouseMotion(int in_x, int in_y);

	void getVectorFromMouse(Vector& i_sourceVect, Vector& i_targetVect);
	void getVectorFromRealPosition(int i_x, int i_y, Vector& i_sourceVect, Vector& i_targetVect);

	WhiteRectangle m_whiteRectangle;

	int m_mousePositionX;
	int m_mousePositionY;

	int m_mousePositionSelectionX;
	int m_mousePositionSelectionY;


private:
	bool getCollison(const Vector& i_vect0, const Vector& i_vect1);
	bool zoneSelection();
	void placeTemplate();
	void generalSelection();
	void unitMoving();

	bool isSelectionRectangleUsed()const;

	CameraState m_cameraState;
	Vector3I m_selectorCorners[2];
	int m_verticalOffset;
	Chunk m_selectorChunk;

	bool m_isSelecting;
};

#endif //CAMERA_RTS_H
