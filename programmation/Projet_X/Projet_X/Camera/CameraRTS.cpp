#include "CameraRTS.h"

#include "../Input.h"
#include "../Entities/Unit.h"
#include "../SceneOpenGL.h"
#include "../Entities/UnitHandler.h"
#include "../Terrain/Terrain.h"
#include "../GUI/GUIHandler.h"
#include "../GUI/RTSGUI.h"
#include "../GUI/TemplateEditorGUI.h"
#include "../Building/TemplateManager.h"
#include "../GUI/MaterialSelectionGUI.h"

#include "../Rendering/DeferredRenderer/DeferredRenderer.h"

CameraRTS::CameraRTS(const Camera& ipk_camera)
:m_cameraState(DEFAULT_STATE), m_selectorChunk(0, 0, 0 ,0 ,0 ,0, false), m_isSelecting(false)
{
	m_isSaving = false;
	m_sensitivity = CAMERA_SENSITIVITY;

	mp_unit = 0;

	m_target = ipk_camera.getPosition();
	m_phi = -45.0f;
	m_theta = -45.0f;

	vectorsFromAngles(true);
	m_position -= m_forward * 5;

	SDL_GetWindowSize(*SceneOpenGL::getInstance()->getWindow(), &m_mousePositionX, &m_mousePositionY);
	m_mousePositionX /= 2;
	m_mousePositionY /= 2;

	init();
}

CameraRTS::CameraRTS()
:m_cameraState(DEFAULT_STATE), m_selectorChunk(0, 0, 0 ,0 ,0 ,0, false), m_isSelecting(false)
{
	mp_unit = 0;
	init();
}

void CameraRTS::init()
{
	GUIHandler* p_guiHandler(GUIHandler::getInstance());
	p_guiHandler->setVisibility(FPS_LAYOUT, false);
	p_guiHandler->setVisibility(RTS_LAYOUT, true);
	p_guiHandler->setVisibility(MAP_LAYOUT, true);
}


CameraRTS::~CameraRTS()
{
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
void CameraRTS::OnMouseMotion(int i_x, int i_y)
{
	Input* p_input(Input::getInstance());
	if(p_input->getInputState() != CONSOLE_INPUT)
	{
		m_mousePositionX = i_x;
		m_mousePositionY = i_y;

		if(p_input->getMouseButState(MOUSE_MIDDLE))
		{
			int centerX, centerY;
			SDL_GetWindowSize(*SceneOpenGL::getInstance()->getWindow(), &centerX, &centerY);
			centerX /= 2;
			centerY /= 2;

			//On calcule la position de la souris par rapport au centre de la fenêtre
			float xRel;
			float yRel;

			if(p_input->getMouseButPress(MOUSE_MIDDLE))
			{
				xRel = 0;
				yRel = 0;
				p_input->centerMouse();
			}
			else
			{
				xRel = (float)(centerX - i_x);
				yRel = (float)(centerY - i_y);
			}

			//On calcule la différence d'angle imposée par la position de la souris
			setTheta(m_theta - xRel * m_sensitivity);
			setPhi(m_phi + yRel * m_sensitivity);

			//On modifie les vecteurs selon les angles
			vectorsFromAngles(true);

			//On replace le curseur au centre de la fenêtre
	
			p_input->centerMouse();
		}
	}
}

void setInsideRTSGUI(int& i_x, int& i_y)
{
	int startRTSMenu(SceneOpenGL::getInstance()->getWindowHeight() - GUIHandler::getInstance()->getLayout(RTS_LAYOUT)->getAbsoluteSize("information").second);
	if(i_y > startRTSMenu)
		i_y = startRTSMenu;
}
bool CameraRTS::getCollison(const Vector& i_vect0, const Vector& i_vect1)
{
	int x[8], y[8];

	Vector vertices[8];

	vertices[0].x = i_vect0.x;
	vertices[0].y = i_vect0.y;
	vertices[0].z = i_vect0.z;

	vertices[1].x = i_vect1.x;
	vertices[1].y = i_vect0.y;
	vertices[1].z = i_vect0.z;

	vertices[2].x = i_vect0.x;
	vertices[2].y = i_vect1.y;
	vertices[2].z = i_vect0.z;

	vertices[3].x = i_vect1.x;
	vertices[3].y = i_vect1.y;
	vertices[3].z = i_vect0.z;
	
	vertices[4].x = i_vect0.x;
	vertices[4].y = i_vect0.y;
	vertices[4].z = i_vect1.z;
	
	vertices[5].x = i_vect1.x;
	vertices[5].y = i_vect0.y;
	vertices[5].z = i_vect1.z;
	
	vertices[6].x = i_vect0.x;
	vertices[6].y = i_vect1.y;
	vertices[6].z = i_vect1.z;
	
	vertices[7].x = i_vect1.x;
	vertices[7].y = i_vect1.y;
	vertices[7].z = i_vect1.z;

	for(int i(0); i < 8; ++i)
		getScreenPosition(vertices[i], x[i], y[i]);

	int minX(INT_MAX), maxX(INT_MIN), minY(INT_MAX), maxY(INT_MIN);
	for(int i(0); i < 8; ++i)
	{
		if(minX > x[i])
			minX = x[i];
		if(maxX < x[i])
			maxX = x[i];
		if(minY > y[i])
			minY = y[i];
		if(maxY < y[i])
			maxY = y[i];
	}
	int mousePositionSelectionX1(m_mousePositionX);
	int mousePositionSelectionY1(m_mousePositionY);
	setInsideRTSGUI(mousePositionSelectionX1, mousePositionSelectionY1);
	if((minX > min(m_mousePositionSelectionX, mousePositionSelectionX1) || maxX > min(m_mousePositionSelectionX, mousePositionSelectionX1)) && 
	(minX < max(m_mousePositionSelectionX, mousePositionSelectionX1) || maxX < max(m_mousePositionSelectionX, mousePositionSelectionX1)) && 
	(minY > min(m_mousePositionSelectionY, mousePositionSelectionY1) || maxY > min(m_mousePositionSelectionY, mousePositionSelectionY1)) && 
	(minY < max(m_mousePositionSelectionY, mousePositionSelectionY1) || maxY < max(m_mousePositionSelectionY, mousePositionSelectionY1)))
		return true;
	return false;
}

bool CameraRTS::zoneSelection()
{
	Input* p_input(Input::getInstance());

	if(p_input->getMouseButRelease(MOUSE_RIGHT))
	{
		m_selectorChunk.removeWhitePrismeDrawable();
		m_cameraState = DEFAULT_STATE;
		return false;
	}

	if(p_input->getKeyRelease("verticalUp") && p_input->getInputState() != CONSOLE_INPUT)
		++m_verticalOffset;
	if(p_input->getKeyRelease("verticalDown") && p_input->getInputState() != CONSOLE_INPUT)
		--m_verticalOffset;

	Vector sourceVect, targetVect;
	getVectorFromMouse(sourceVect, targetVect);
	std::vector<Vector3I> listOfCube(Terrain::getInstance()->getCollision(sourceVect, targetVect, true));
	if(listOfCube.size() != 0)
	{
		if(m_cameraState == ZONE_SELECTING1_TEMPLATE_STATE || m_cameraState == ZONE_SELECTING1_FILLER_STATE)
		{
			m_selectorCorners[0].x  = listOfCube[0].x;
			m_selectorCorners[0].y  = listOfCube[0].y + m_verticalOffset;
			m_selectorCorners[0].z  = listOfCube[0].z;

			m_selectorCorners[1].y  = m_selectorCorners[0].y;
		}
		else
		{
			m_selectorCorners[1].y  = m_selectorCorners[0].y + m_verticalOffset;
		}
		m_selectorCorners[1].x  = listOfCube[0].x;
		m_selectorCorners[1].z  = listOfCube[0].z;


		int sizeX(m_selectorCorners[1].x - m_selectorCorners[0].x + ((m_selectorCorners[1].x >= m_selectorCorners[0].x)?1:-1));
		int sizeY(m_selectorCorners[1].y - m_selectorCorners[0].y + ((m_selectorCorners[1].y >= m_selectorCorners[0].y)?1:-1)); 
		int sizeZ(m_selectorCorners[1].z - m_selectorCorners[0].z + ((m_selectorCorners[1].z >= m_selectorCorners[0].z)?1:-1));
		m_selectorChunk.resizeChunk(abs(sizeX), abs(sizeY), abs(sizeZ));
		m_selectorChunk.moveChunk((sizeX > 0)?m_selectorCorners[0].x:m_selectorCorners[1].x, 
								  (sizeY > 0)?m_selectorCorners[0].y:m_selectorCorners[1].y, 
								  (sizeZ > 0)?m_selectorCorners[0].z:m_selectorCorners[1].z);
	}
	if(p_input->getMouseButRelease(MOUSE_LEFT))
	{
		m_verticalOffset = 0;
		if(m_cameraState == ZONE_SELECTING2_TEMPLATE_STATE || m_cameraState == ZONE_SELECTING2_FILLER_STATE)
		{
			m_selectorChunk.removeWhitePrismeDrawable();
		}
		switch(m_cameraState)
		{
			case ZONE_SELECTING1_TEMPLATE_STATE:
				m_cameraState = ZONE_SELECTING2_TEMPLATE_STATE;
				break;
			case ZONE_SELECTING1_FILLER_STATE:
				m_cameraState = ZONE_SELECTING2_FILLER_STATE;
				break;
			default:
			{
				m_cameraState = DEFAULT_STATE;
				return true;
			}
		}
	}
	return false;
}

void CameraRTS::placeTemplate()
{
	Input* p_input(Input::getInstance());
	TemplateManager* p_templateManager(TemplateManager::getInstance());

	//On annule le placement
	if(p_input->getMouseButPress(MOUSE_RIGHT))
	{
		p_templateManager->getCurrentTemplateShape()->removeWhitePrismeDrawable();
		p_templateManager->clearCurrentTemplateShape();
		m_cameraState = DEFAULT_STATE;
		return;
	}
	//Si on appuie sur le bouton gauche, on place le template
	if(p_input->getMouseButRelease(MOUSE_LEFT))
	{
		
		if(!p_templateManager->isInCollision(p_templateManager->getCurrentTemplateShape()))
		{
			int templateID(p_templateManager->createTemplate(p_templateManager->getCurrentTemplateShape()));
			if(mp_selectedUnits.size() > 0)
			{
				UnitHandler* p_unitHandler(UnitHandler::getInstance());
				for(std::hash_set<Unit*>::iterator it(mp_selectedUnits.begin()), selectedUnitsEnd(mp_selectedUnits.end()); it != selectedUnitsEnd; ++it)
				{
					p_unitHandler->moveTemplateAction((*it)->m_teamID, (*it)->m_unitID, p_templateManager->getTemplate(templateID));
					p_unitHandler->buildTemplateAction((*it)->m_teamID, (*it)->m_unitID, p_templateManager->getTemplate(templateID));
				}
			}
		}
		p_templateManager->getCurrentTemplateShape()->removeWhitePrismeDrawable();
		p_templateManager->clearCurrentTemplateShape();
		m_cameraState = DEFAULT_STATE;
	}
	else
	{
		//On affiche le template
		Vector sourceVect, targetVect;
		getVectorFromMouse(sourceVect, targetVect);

		std::vector<Vector3I> listOfCube(Terrain::getInstance()->getCollision(sourceVect, targetVect, true));
		if(listOfCube.size() != 0)
		{
			++listOfCube[0].y;
			p_templateManager->moveCurrentTemplateShapeBottomCenter(listOfCube[0]);

			Chunk* tempShape(TemplateManager::getInstance()->getCurrentTemplateShape());
			char cubeStr[100];
			sprintf(cubeStr, "\nTemplatePos: (%i,%i,%i)", tempShape->getCubePosition().x, tempShape->getCubePosition().y, tempShape->getCubePosition().z);
			GUIHandler::getInstance()->addToDebugHUD(cubeStr);
		}
	}
}

bool isInsideRTSGUI(int i_x, int i_y)
{
	int startRTSMenu(SceneOpenGL::getInstance()->getWindowHeight() - GUIHandler::getInstance()->getLayout(RTS_LAYOUT)->getAbsoluteSize("information").second);
	if(i_y > startRTSMenu)
		return true;
	return false;
}

void CameraRTS::generalSelection()
{
	Input* p_input(Input::getInstance());
	TemplateManager* p_templateManager(TemplateManager::getInstance());
	//Sélection des unitées et templates
	if(p_input->getMouseButPress(MOUSE_LEFT))
	{
		
		if(!isInsideRTSGUI(m_mousePositionX, m_mousePositionY))
		{
			m_isSelecting = true;
			m_mousePositionSelectionX = m_mousePositionX;
			m_mousePositionSelectionY = m_mousePositionY;
		}
	}
	if(p_input->getMouseButRelease(MOUSE_LEFT))
		m_isSelecting = false;


	if(p_templateManager->getCurrentTemplateShape() == 0)
	{
		if(m_isSelecting)
		{

			UnitHandler* p_unitHandler(UnitHandler::getInstance());
			TemplateManager* p_templateManager(TemplateManager::getInstance());

			if(!p_input->getKeyState("addRTSAction"))
			{
				std::hash_set<Unit*> emptySelectedUnits;
				mp_selectedUnits.swap(emptySelectedUnits);
				p_unitHandler->deselectAll();

				std::hash_set<Template*> emptySelectedTemplates;
				mp_selectedTemplates.swap(emptySelectedTemplates);
				p_templateManager->deselectAll();
			}
		
			//Selection des unitées
			Unit* p_unit;
			Vector positionUnit[2];
			for(int i(0), unitCount(p_unitHandler->getUnitCount(0)); i < unitCount; ++i)
			{
				p_unit = p_unitHandler->getUnit(0, i);
			
				positionUnit[0].x = p_unit->getPosition().x - p_unit->m_hitBox.getDimension(WIDTH)/2.f;
				positionUnit[0].y = p_unit->getPosition().y - p_unit->m_hitBox.getDimension(HEIGHT)/2.f;
				positionUnit[0].z = p_unit->getPosition().z - p_unit->m_hitBox.getDimension(LENGTH)/2.f;

				positionUnit[1].x = p_unit->getPosition().x + p_unit->m_hitBox.getDimension(WIDTH)/2.f;
				positionUnit[1].y = p_unit->getPosition().y + p_unit->m_hitBox.getDimension(HEIGHT)/2.f;
				positionUnit[1].z = p_unit->getPosition().z + p_unit->m_hitBox.getDimension(LENGTH)/2.f;

				if(getCollison(positionUnit[0], positionUnit[1]))
				{
					p_unitHandler->setIsSelected(p_unit, true);
					mp_selectedUnits.insert(p_unit);
				}
			}
			//Selection des templates
			Template* p_template;
			Vector positionTemplate[2];
			for(int i(0), templateCount(p_templateManager->getTemplateCount()); i < templateCount; ++i)
			{
				p_template = p_templateManager->getTemplate(i);
			
				positionTemplate[0].x = p_template->getPosition().x;
				positionTemplate[0].y = p_template->getPosition().y;
				positionTemplate[0].z = p_template->getPosition().z;

				positionTemplate[1].x = p_template->getPosition().x + p_template->getDimension(WIDTH);
				positionTemplate[1].y = p_template->getPosition().y + p_template->getDimension(HEIGHT);
				positionTemplate[1].z = p_template->getPosition().z + p_template->getDimension(LENGTH);

				if(getCollison(positionTemplate[0], positionTemplate[1]))
				{
					p_templateManager->setIsSelected(i, true);
					mp_selectedTemplates.insert(p_template);
				}
			}
		}
	}
}

void CameraRTS::unitMoving()
{
	Input* p_input(Input::getInstance());
	//Déplacement des unitées selectionnés
	if( p_input->getMouseButRelease(MOUSE_RIGHT) && !mp_selectedUnits.empty() && p_input->getInputState() != MAP_INPUT)
	{
		//On trouve le cube pointé
		Vector sourceVect, targetVect;
		getVectorFromMouse(sourceVect, targetVect);
		std::vector<Vector3I> listOfCube(Terrain::getInstance()->getCollision(sourceVect, targetVect, true));

		if(listOfCube.size() != 0)
		{
			++listOfCube[0].y;
			unitMoving(listOfCube[0], TemplateManager::getInstance()->templateInCollision(sourceVect, targetVect));
		}
	}
}

void CameraRTS::unitMoving(const Vector3I& i_cubeTarget, int i_templateID)
{
	if(i_templateID == -1)
	{
		UnitHandler* p_unitHandler(UnitHandler::getInstance());
		int groupID(p_unitHandler->createBoidGroup(mp_selectedUnits));
		p_unitHandler->moveBoidGroup(groupID, i_cubeTarget);
	}
	else
	{
		UnitHandler* p_unitHandler(UnitHandler::getInstance());
		TemplateManager* p_templateManager(TemplateManager::getInstance());
		for(std::hash_set<Unit*>::iterator it(mp_selectedUnits.begin()), selectedUnitsEnd(mp_selectedUnits.end()); it != selectedUnitsEnd; ++it)
		{
			p_unitHandler->moveTemplateAction((*it)->m_teamID, (*it)->m_unitID, p_templateManager->getTemplate(i_templateID));
			p_unitHandler->buildTemplateAction((*it)->m_teamID, (*it)->m_unitID, p_templateManager->getTemplate(i_templateID));
		}
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
void CameraRTS::animate(Uint32 i_timestep)
{
	m_lastSelectorState = getSelectorState();
	UnitHandler::getInstance()->updateHitpointBar();
	TemplateManager::getInstance()->updateHitpointBar();

	Input* p_input(Input::getInstance());
	
	//Changement de caméra
	if(p_input->getInputState() != CONSOLE_INPUT)
	{
		if(p_input->getKeyPress("changeView") && !mp_selectedUnits.empty())
		{
			p_input->setCameraType(FPS_CAMERA);
		}
		//Rotation de la caméra
		if(p_input->getMouseButPress(MOUSE_MIDDLE))
			p_input->centerMouse();

		//On calcule le mouvement de la caméra
		Vector movement(0.0f, 0.0f, 0.0f);
		if(p_input->getKeyState("up"))
			movement += m_walkingDirection;
		if(p_input->getKeyState("down"))
			movement -= m_walkingDirection;
		if(p_input->getKeyState("left"))
			movement += m_left;
		if(p_input->getKeyState("right"))
			movement -= m_left;
		if(m_cameraState != ZONE_SELECTING0_TEMPLATE_STATE && m_cameraState != ZONE_SELECTING1_TEMPLATE_STATE && m_cameraState != ZONE_SELECTING2_TEMPLATE_STATE &&
			m_cameraState != ZONE_SELECTING0_FILLER_STATE && m_cameraState != ZONE_SELECTING1_FILLER_STATE && m_cameraState != ZONE_SELECTING2_FILLER_STATE)
		{
			if(p_input->getKeyState("verticalUp") && p_input->getInputState() != MAP_INPUT)
				movement += Vector(0, 1, 0);
			if(p_input->getKeyState("verticalDown") && p_input->getInputState() != MAP_INPUT)
				movement -= Vector(0, 1, 0);
		}
		movement.normalize();

		movement *= 0.01f * i_timestep;

		setTarget(m_target + movement);
		setPositionFromTarget();

		switch(m_cameraState)
		{
			case DEFAULT_STATE:
				if(p_input->getKeyState("materialSelection"))
					GUIHandler::getInstance()->setVisibility(MATERIAL_SELECTION_LAYOUT, true);
				unitMoving();
				generalSelection();
				break;

			case ZONE_SELECTING0_FILLER_STATE:
				m_cameraState = ZONE_SELECTING1_FILLER_STATE;
				break;
			case ZONE_SELECTING1_FILLER_STATE:
			case ZONE_SELECTING2_FILLER_STATE:
				if(zoneSelection())
				{
					Chunk* p_filledChunk(new Chunk(m_selectorChunk));
					p_filledChunk->fill(((MaterialSelectionGUI*)GUIHandler::getInstance()->getLayout(MATERIAL_SELECTION_LAYOUT))->getCurrentCubeType(), 100);
					TemplateManager::getInstance()->createTemplate(p_filledChunk, true);
				}
				break;

			case ZONE_SELECTING0_TEMPLATE_STATE:
				m_cameraState = ZONE_SELECTING1_TEMPLATE_STATE;
				break;
			case ZONE_SELECTING1_TEMPLATE_STATE:
			case ZONE_SELECTING2_TEMPLATE_STATE:
				if(zoneSelection())
				{
					GUIHandler::getInstance()->setVisibility(TEMPLATE_EDITOR_LAYOUT, true);
					((TemplateEditorGUI*)GUIHandler::getInstance()->getLayout(TEMPLATE_EDITOR_LAYOUT))->addSelectorChunk(&m_selectorChunk);
				}
				break;
			case PRE_TEMPLATE_SELECTED_STATE:
					m_cameraState = TEMPLATE_SELECTED_STATE;
				break;
			case TEMPLATE_SELECTED_STATE:
				placeTemplate();
				break;
			case CONSOLE_STATE:
			default:
				break;
		
		}
	}

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
void CameraRTS::update()
{
	Input* p_input(Input::getInstance());
	int n_x = p_input->getMousePosX();
	int n_y = p_input->getMousePosY();
	OnMouseMotion(n_x, n_y);
}

void CameraRTS::draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const
{
	Input* p_input(Input::getInstance());
	if(m_isSelecting)
	{

		const float nearRenderingDistanceNear(RENDERING_DISTANCE_NEAR + RENDERING_DISTANCE_NEAR * 0.001);
		i_modelview.push();
		

		int windowWidth, windowHeight;
		SDL_GetWindowSize(*SceneOpenGL::getInstance()->getWindow(), &windowWidth, &windowHeight);

		int centerX = windowWidth/2;
		int centerY = windowHeight/2;

		float tanRadAngle(tan(MATH_PI * (DEFAULT_VIEW_ANGLE/2.0f)/180.0f));
		float virtualHeightNear(2 * tanRadAngle * nearRenderingDistanceNear);

		float windowRatio((float)windowWidth / (float)windowHeight);
		float virtualWidthNear(windowRatio * virtualHeightNear);

		float xTranslate((((float)(m_mousePositionSelectionX + m_mousePositionX)/2.0f) - (float)centerX)/((float)windowWidth/2.0f));
		float yTranslate((((float)(m_mousePositionSelectionY + m_mousePositionY)/2.0f) - (float)centerY)/((float)windowHeight/2.0f));

		float xScale((float)abs(m_mousePositionSelectionX - m_mousePositionX)/((float)windowWidth/2.0f));
		float yScale((float)abs(m_mousePositionSelectionY - m_mousePositionY)/((float)windowHeight/2.0f));

		Matrix projection;
		projection.loadIdentity();
		Matrix modelview;
		modelview.loadIdentity();

		modelview.translate(xTranslate, -yTranslate, 0);

		modelview.scale(xScale, yScale, 1.0f);		

		m_whiteRectangle.draw(projection, modelview, ipk_camera);

		i_modelview.pop();
	}
}

void CameraRTS::getVectorFromMouse(Vector& i_sourceVect, Vector& i_targetVect)
{
	CameraRTS::getVectorFromRealPosition(m_mousePositionX, m_mousePositionY, i_sourceVect, i_targetVect);
}

void CameraRTS::getVectorFromRealPosition(int i_x, int i_y, Vector& i_sourceVect, Vector& i_targetVect)
{
	//On trouve les dimensions de la fenêtre virtuel et réel
	int windowWidth, windowHeight;
	SDL_GetWindowSize(*SceneOpenGL::getInstance()->getWindow(), &windowWidth, &windowHeight);
	int centerX(windowWidth/2);
	int centerY(windowHeight/2);
	float windowRatio((float)windowWidth / (float)windowHeight);

	float tanRadAngle(tan(MATH_PI * (DEFAULT_VIEW_ANGLE/2.0f)/180.0f));

	float virtualHeightFar(2 * tanRadAngle * RENDERING_DISTANCE_FAR);
	float virtualWidthFar(windowRatio * virtualHeightFar);

	float virtualHeightNear(2 * tanRadAngle * RENDERING_DISTANCE_NEAR);
	float virtualWidthNear(windowRatio * virtualHeightNear);

	//On creer les vecteurs sous la souris
	//On trouve la grandeur de la fenêtre virtuel au maximum de la distance rendu


	float virtualXRelativeFar(((centerX - i_x) * virtualWidthFar)/windowWidth);
	float virtualZRelativeFar(((centerY - i_y) * virtualHeightFar)/windowHeight);

	float virtualXRelativeNear(((centerX - i_x) * virtualWidthNear)/windowWidth);
	float virtualZRelativeNear(((centerY - i_y) * virtualHeightNear)/windowHeight);

	Vector up(m_forward.crossProduct(m_left));
	up.normalize();

	i_sourceVect = m_position + m_forward * RENDERING_DISTANCE_NEAR;
	i_sourceVect += m_left * virtualXRelativeNear;
	i_sourceVect += up * virtualZRelativeNear;

	i_targetVect = m_position + m_forward * RENDERING_DISTANCE_FAR;
	i_targetVect += m_left * virtualXRelativeFar;
	i_targetVect += up * virtualZRelativeFar;
}

void CameraRTS::selectCurrentTemplateShape(int i_index)
{
	m_cameraState = PRE_TEMPLATE_SELECTED_STATE;
	TemplateManager* p_templateManager(TemplateManager::getInstance());
	if(p_templateManager->getCurrentTemplateShape() != 0)
	{
		p_templateManager->getCurrentTemplateShape()->removeWhitePrismeDrawable();
		p_templateManager->clearCurrentTemplateShape();
	}
	
	p_templateManager->setCurrentTemplateShape(((RTSGUI*)GUIHandler::getInstance()->getLayout(RTS_LAYOUT))->getTemplateShape(i_index));
	p_templateManager->getCurrentTemplateShape()->addWhitePrismeDrawable();
	DeferredRenderer::getInstance()->addDrawable(p_templateManager->getCurrentTemplateShape());
}

bool CameraRTS::isSelectionRectangleUsed()const
{
	switch(m_cameraState)
	{
		case DEFAULT_STATE:
			return true;
		default:
			return false;
	}
}

void CameraRTS::switchToTemplateSelectionMode()
{
	Input* p_input(Input::getInstance());
	if(m_cameraState == DEFAULT_STATE)
	{
		m_cameraState = ZONE_SELECTING0_TEMPLATE_STATE;
		m_verticalOffset = 0;
		m_selectorChunk.addWhitePrismeDrawable();
	}
}

void CameraRTS::switchToFillSelectionMode()
{
	Input* p_input(Input::getInstance());
	if(m_cameraState == DEFAULT_STATE)
	{
		m_cameraState = ZONE_SELECTING0_FILLER_STATE;
		m_verticalOffset = 0;
		m_selectorChunk.addWhitePrismeDrawable();
	}
}