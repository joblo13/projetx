#include "SceneOpenGL.h"

//include externe
#define ILUT_USE_OPENGL	// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "Texture.h"
#include "Terrain/Terrain.h"
#include "Terrain/SkyBox.h"


//Include interne
#include "Input.h"
#include "Math/Matrix.h"
#include "Camera/Camera.h"
#include "Entities/UnitHandler.h"
#include "Entities/Unit.h"
#include "GUI/GUIHandler.h"
#include "GUI/RTSGUI.h"


#include "Shaders/Shader.h"
#include "AI/AStar.h"
#include "AI/Node.h"
#include "HitBox.h"
#include "Building/Building.h"
#include "Rendering/DeferredRenderer/DeferredRenderer.h"

#include "Bullet/btBulletDynamicsCommon.h"

#include "Debugger/GlDebugger.h"

//Initialisation de l'instance
SceneOpenGL* SceneOpenGL::mp_instance(0);

////////////////////////////////////////////////////////////////////////
///
/// @fn SceneOpenGL(const std::string& ik_windowTitle, int i_windowWidth, int i_windowHeight)
///
/// Constructeur
/// 
/// @param[in] ik_windowTitle: Titre de la fenêtre à créer
/// @param[in] i_windowWidth: Largeur de la fenêtre (en pixel)
/// @param[in] i_windowHeight: Hauteur de la fenêtre (en pixel)
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
SceneOpenGL::SceneOpenGL(const std::string& ik_windowTitle, int i_windowWidth, int i_windowHeight)
: mstr_windowTitle(ik_windowTitle), m_windowWidth(i_windowWidth), m_windowHeight(i_windowHeight), m_openGLContext(0), m_displayMode(WINDOW), m_close(false), m_inMainMenu(true),
mp_hitpointBarMesh(0), mp_camera(0)
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~SceneOpenGL()
///
/// Destructeur
/// Detruit le contexte openGL, la fenetre et ferme la  SDL
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
SceneOpenGL::~SceneOpenGL()
{
	if(mp_hitpointBarMesh != 0)
		 delete mp_hitpointBarMesh;
	//On detruit le contexte opengl
    SDL_GL_DeleteContext(m_openGLContext);
	//On detruit la fenêtre
    SDL_DestroyWindow(m_window);
	//On ferme la SDL
    SDL_Quit();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void init()
///
/// Initialisation de l'environnement
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void SceneOpenGL::init()
{  
	// On choisi la version d'opengl
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Initialisation de la SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
		//Si une erreur se produit, on l'imprime
        std::cout << "Erreur lors de l'initialisation de SDL : " << SDL_GetError() << std::endl;
		//On ferme la SDL
        SDL_Quit();
		exit(-15);
    }

    // On paramètre le double buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Création de la fenêtre
    m_window = SDL_CreateWindow(mstr_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    m_displayMode = WINDOW;
	m_openGLContext = SDL_GL_CreateContext(m_window);
	//On choisi l'interval de swap
	SDL_GL_SetSwapInterval(1);
 
	//Initialisation de openGL
	if(!initGL())
		exit(-1);

	// On vérifie si DevIL est bien initialisé
	if ( (iluGetInteger(IL_VERSION_NUM) < IL_VERSION) || (iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) || (ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) )
	{
		std::cout << "DevIL versions are different... Exiting." << std::endl;
		exit(-1);
	}

	//On intialise les fonctions suplémentaire de DevIL
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);	// Tell DevIL that we're using OpenGL for our rendering
 
	GUIHandler::getInstance();
	glGetError();
	mp_hitpointBarMesh = new HitpointBarMesh();
	mp_hitpointBarMesh->addTexture("TextureSampler", "Texture/hitpoint_bar.png");
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool initGL()
///
/// Fonction initialisant le contexte openGL
///
/// @return Booléen confirmant que le contexte openGL à pu être initialisé
///
////////////////////////////////////////////////////////////////////////
bool SceneOpenGL::initGL()
{
	//On trouve une version d'OpenGL compatible avec la carte graphique
	bool got_gl(false);
	for(int index(0); index < SDL_GetNumRenderDrivers(); ++index)
	{
		printf("Getting info on driver %d:\n", index);
		SDL_RendererInfo info;
		if(SDL_GetRenderDriverInfo(index, &info) != -1)
		{
			printf("Driver = %s, index: %d\n", info.name, index);
			
			if (!strcmp(info.name, "opengl"))
			{
				printf("Attempting to select this driver...\n");
				m_renderer = SDL_CreateRenderer(m_window, index, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
				if( m_renderer == 0)
				{
					printf("Failed to get OpenGL backend\n");
					printf("Destroying Window and Shutting Down SDL\n");
					SDL_DestroyWindow(m_window);
					SDL_Quit();
					return false;
				}
				else
				{
					got_gl = true;
					break;
				}
			}
		}
	}
	if (!got_gl)
	{
		printf("Failed to get OpenGL backend\n");
		printf("Destroying Window and Shutting Down SDL\n");
		SDL_DestroyWindow(m_window);
		SDL_Quit();
		return false;
	}


	//On effectue cette partie seulement si l'ordinateur est un pc
    #ifdef WIN32

    // On initialise GLEW
    GLenum initialisationGLEW( glewInit() );


    // Si l'initialisation a échoué :

    if(initialisationGLEW != GLEW_OK)
    {
        // On affiche l'erreur grâce à la fonction : glewGetErrorString(GLenum code)
        std::cout << "Erreur d'initialisation de GLEW : " << glewGetErrorString(initialisationGLEW) << std::endl;

        // On quitte la SDL
        SDL_GL_DeleteContext(m_openGLContext);
        SDL_DestroyWindow(m_window);
        SDL_Quit();

        return false;
    }
    #endif
	//Sinon, on doit utiliser une autre librairie


    //On modifie les paramètres d'opengl
    OpenGLHandler::enable(GL_DEPTH_TEST);
	OpenGLHandler::enable(GL_TEXTURE_2D);
	OpenGLHandler::enable(GL_CULL_FACE);
	OpenGLHandler::cullFace(GL_FRONT);

    return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void injectTimePulseCEGUI(float& i_lastTimePulse)
///
/// Fonction permettant d'envoyer le pas de temps à l'interface graphique
///
/// @param[in] i_lastTimePulse: pas de temps
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void injectTimePulseCEGUI(float& i_lastTimePulse)
{
	// get current "run-time" in seconds
	float t = 0.001*SDL_GetTicks();
 
	// inject the time that passed since the last call 
	bool test = CEGUI::System::getSingleton().injectTimePulse(t-i_lastTimePulse);

	GUIHandler::getInstance()->addToDebugHUD(t-i_lastTimePulse);
 
	// store the new time as the last time
	i_lastTimePulse = t;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void toggleDisplayMode()
///
/// Fonction inversant le mode d'affichage
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void SceneOpenGL::toggleDisplayMode()
{
	setDisplayMode(!m_displayMode);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void setDisplayMode(int i_displayMode)
///
/// Fonction permettant de choisir le mode d'affichage 
///
/// @param[in] i_displayMode: mode d'affichage
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void SceneOpenGL::setDisplayMode(int i_displayMode)
{
	switch(i_displayMode)
	{
		case FULLSCREEN:
			SDL_SetWindowFullscreen(m_window, SDL_TRUE);
			break;
		case WINDOW:
			SDL_SetWindowFullscreen(m_window, SDL_FALSE);
			break;
	}
	m_displayMode = i_displayMode;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void initTerrain()
///
/// Fonction créant le terrain
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void SceneOpenGL::initTerrain()
{
	//On crée le terrain
	Terrain* p_terrain(Terrain::getInstance());
	p_terrain->setDimension(1, 3, 1);
	p_terrain->loadMap(10);

	//On ajoute des unitées
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	
	p_unitHandler->addHuman(0, Vector3I(8, 50, 8));
	

	//for(int i(0); i < 2; ++i)
	//	for(int j(0); j < 2; ++j)
	//		p_unitHandler->addHuman(0, Vector3I(10 + 2*i, 50, 10 + 2*j));

	p_terrain->loadMap("test.map");
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void mainMenuLoop()
///
/// Fonction s'exécutant en continue lorsque l'on est dans les menus
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void SceneOpenGL::mainMenuLoop()
{
	//On crée les deux matrices
 	Matrix projection, modelview;
    projection.loadIdentity();
    modelview.loadIdentity();

	//On crée une limite pour le nombre d'image par seconde
    unsigned int frameRate (1000 / 50);
	
	//On crée le gestionnaire des inputs
	Input* p_input(Input::getInstance());

	GUIHandler* p_guiHandler(GUIHandler::getInstance());

	//On crée des compteurs de tick et de fps
	Uint32 lastTicks(0), currentTicks(0), loopTimeLength(0), fpsCounterTicks(0);
	int fpsCount(0), fpsLastCount(0);

	int teamID(0);
	Camera* p_camera(0);

	//On affiche en continue jusqu'à ce que 
	float lastTimePulse = 0.001f * static_cast<float>(SDL_GetTicks());

	while(m_inMainMenu)
	{   
		//On vide les buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//On prend en compte les nouveaux inputs
		p_input->updateEvent();

		//On échange l'image dans le buffer avec l'image actuel

		injectTimePulseCEGUI(lastTimePulse);

		p_guiHandler->updateLayout(loopTimeLength, teamID, p_camera);
		p_guiHandler->draw();

		// Update the screen
		SDL_RenderPresent(m_renderer);

		//On enregistre le nombre de ticks de la dernière execution
		lastTicks = currentTicks;
		//On enregistre le nouveau nombre de ticks
		currentTicks = SDL_GetTicks();
		//On calcule le nombre de temps pris pour effectuer les actions de la dernière frame
		loopTimeLength = currentTicks - lastTicks;

        //Si le temps n'est pas suffisant, on attent le temps restant
        if(loopTimeLength < frameRate)
            SDL_Delay(frameRate - loopTimeLength);
	}
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void gameLoop()
///
/// Fonction s'exécutant en continue durant l'exécution du programme lorsque l'on est dans une partie
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void SceneOpenGL::gameLoop()
{
	DeferredRenderer* p_deferredRenderer = DeferredRenderer::getInstance();

	//On crée les deux matrices
	Matrix projection, modelview;
	projection.loadIdentity();
    modelview.loadIdentity();

	//On crée une limite pour le nombre d'image par seconde
    unsigned int frameRate (1000 / 50);
	
	//On crée le gestionnaire des inputs
	Input* p_input(Input::getInstance());

	//On crée le terrain
	Terrain* p_terrain(Terrain::getInstance());
	
	//On crée la skybox
	//SkyBox skyBox = SkyBox(SKYBOX_SHADER);
	
	UnitHandler* p_unitHandler(UnitHandler::getInstance());

	//On crée la caméra
	mp_camera = new Camera();
	mp_camera->setUnit(p_unitHandler->getUnit(0, 0));

	GUIHandler* p_guiHandler(GUIHandler::getInstance());
	p_guiHandler->setVisibility(FPS_LAYOUT, true);
	int teamID(0);

	//On crée des compteurs de tick et de fps
	Uint32 lastTicks(0), currentTicks(0), loopTimeLength(0), fpsCounterTicks(0);
	int fpsCount(0), fpsLastCount(0);

	//On affiche en continue jusqu'à ce que 
	float lastTimePulse = 0.001f * static_cast<float>(SDL_GetTicks());
    while(!m_close)
    {
		//On vide les buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//On prend en compte les nouveaux inputs
		p_input->updateEvent();
		p_input->changeCameraType(mp_camera);

		//On anime les éléments de la map
		mp_camera->animate(loopTimeLength);
		p_unitHandler->animate(loopTimeLength);	
		p_deferredRenderer->animate(loopTimeLength);

		//detecter les collisions
		detectCollisions();

		p_deferredRenderer->draw();


		//On incremente le compteur de fps
		++fpsCount;
		//Si la différence entre le tick actuelle et le dernier tick où le fps à été calculé plus grand que 1000
		//On effectue un nouveau calcule de fps
		if(SDL_GetTicks() - fpsCounterTicks >= 1000)
		{
			//On enregistre en mémoire le dernier nombre de fps calculé
			fpsLastCount = fpsCount;
			//On remet le compteur de fps à zéro
			fpsCount = 0;
			//On garde en mémoire la derniere fois que l'on à mis à jour le compte de fps
			fpsCounterTicks = SDL_GetTicks();
		}

		//On échange l'image dans le buffer avec l'image actuel
		p_guiHandler->updateLayout(loopTimeLength, teamID, mp_camera);
		p_guiHandler->draw();

		injectTimePulseCEGUI(lastTimePulse);
								
		if(p_guiHandler->getVisibility(DEBUG_HUD_LAYOUT))
		{
			Vector cameraPosition(mp_camera->getPosition());
			p_guiHandler->addToDebugHUD("FPS:");
			p_guiHandler->addToDebugHUD(fpsLastCount);
			p_guiHandler->addToDebugHUD("\nX:");
			p_guiHandler->addToDebugHUD(cameraPosition.x);
			p_guiHandler->addToDebugHUD("\nY:");
			p_guiHandler->addToDebugHUD(cameraPosition.y);
			p_guiHandler->addToDebugHUD("\nZ:");
			p_guiHandler->addToDebugHUD(cameraPosition.z);

			Vector cameraTarget(mp_camera->getTarget());
			p_guiHandler->addToDebugHUD("\nTarget X:");
			p_guiHandler->addToDebugHUD(cameraTarget.x);
			p_guiHandler->addToDebugHUD("\nTarget Y:");
			p_guiHandler->addToDebugHUD(cameraTarget.y);
			p_guiHandler->addToDebugHUD("\nTarget Z:");
			p_guiHandler->addToDebugHUD(cameraTarget.z);

			p_guiHandler->addToDebugHUD("\nXCube:");
			p_guiHandler->addToDebugHUD(Terrain::positionToCube(cameraPosition.x));
			p_guiHandler->addToDebugHUD("\nYCube:");
			p_guiHandler->addToDebugHUD(Terrain::positionToCube(cameraPosition.y));
			p_guiHandler->addToDebugHUD("\nYCubeFeet:");
			p_guiHandler->addToDebugHUD(Terrain::positionToCube(cameraPosition.y - HALFSIDE * 3.0f));
			p_guiHandler->addToDebugHUD("\nZCube:");
			p_guiHandler->addToDebugHUD(Terrain::positionToCube(cameraPosition.z));

			p_guiHandler->addToDebugHUD("\nPhi:");
			p_guiHandler->addToDebugHUD(mp_camera->getPhi());
			p_guiHandler->addToDebugHUD("\nTheta:");
			p_guiHandler->addToDebugHUD(mp_camera->getTheta());
			char lastDestroyedCube[100];
			sprintf(lastDestroyedCube, "\nLast destroyed cube: (%i,%i,%i)", p_terrain->lastDestroyedCube.x, p_terrain->lastDestroyedCube.y, p_terrain->lastDestroyedCube.z);
			p_guiHandler->addToDebugHUD(lastDestroyedCube);

			p_guiHandler->printDebugHUD();
		}

		// Update the screen
		SDL_RenderPresent(m_renderer);

		//On enregistre le nombre de ticks de la dernière execution
		lastTicks = currentTicks;
		//On enregistre le nouveau nombre de ticks
		currentTicks = SDL_GetTicks();
		//On calcule le nombre de temps pris pour effectuer les actions de la dernière frame
		loopTimeLength = currentTicks - lastTicks;

        //Si le temps n'est pas suffisant, on attent le temps restant
        if(loopTimeLength < frameRate)
            SDL_Delay(frameRate - loopTimeLength);
	}

	DeferredRenderer::releaseInstance();

	//Le programme se termine
	OpenGLHandler::disable(GL_DEPTH_TEST);
	OpenGLHandler::disable(GL_TEXTURE_2D);
	OpenGLHandler::disable(GL_CULL_FACE);
}

Camera* SceneOpenGL::getCamera()
{
	return mp_camera;
}

//pour détecter les collisions, p-e mieux dans le callBack interne...
void SceneOpenGL::detectCollisions()
{
	/*#define GROUND_CST 21
	
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	btDynamicsWorld* collisionWorld(Terrain::getInstance()->getDynamicWorld());

	int teamCount = p_unitHandler->getTeamCount();
	int unitCount;
	Unit* p_unit;

	for (int i(0), numManifolds(collisionWorld->getDispatcher()->getNumManifolds()); i < numManifolds; ++i)
	{
		
		btPersistentManifold* contactManifold = collisionWorld->getDispatcher()->getManifoldByIndexInternal(i);
		
		int numContacts = contactManifold->getNumContacts();

		if (numContacts == 0)
			continue;

		btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

		btCollisionObject* unitObj;
		bool unitIsA = true;
		if(obA->getCollisionShape()->getShapeType() != GROUND_CST)
		{
			if (obB->getCollisionShape()->getShapeType() != GROUND_CST)
			{
				//TODO : deux units qui se frappent...
				continue;
			}
			else 
			{
				unitObj = obA;
			}
		}
		else if (obB->getCollisionShape()->getShapeType() != GROUND_CST)
		{
			unitObj = obB;
			unitIsA = false;
		}
		else
		{
			unitObj = 0;
		}

		bool unitFound = false;
		//On itère pour tous les unités
		for(int i(0); i < teamCount; ++i)
		{
			if (unitFound)
				break;

			unitCount = p_unitHandler->getUnitCount(i);
			for(int j(0); j < unitCount; ++j)
			{
				p_unit = p_unitHandler->getUnit(i,j);
				if(p_unit != 0)
				{	
					if (unitObj == p_unit->getPhysicBody())
					{
						unitFound = true;
						break;
					}
				}
			}
		}

		if (unitFound)
		{
			btVector3 collisionVector(0,0,0);
			for (int j(0), numContacts(contactManifold->getNumContacts()); j < numContacts; ++j)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);

				if (unitIsA)
				{
					collisionVector += pt.m_normalWorldOnB;
				}
				else
				{
					collisionVector -= pt.m_normalWorldOnB;
				}
			}
			
			GUIHandler::getInstance()->addToDebugHUD("collision");
			//on set le vecteur
			if (collisionVector.norm() != 0)
			{
				collisionVector.normalize();			
				p_unit->setCollisionVector(Vector(collisionVector.getX(), collisionVector.getY(), collisionVector.getZ()));
			}
		}
	}*/
}

HitpointBarMesh* SceneOpenGL::getHitpointBarMesh()
{
	return mp_hitpointBarMesh;
}