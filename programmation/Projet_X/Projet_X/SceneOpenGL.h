#ifndef SCENE_OPENGL_H
#define SCENE_OPENGL_H

//Include externe
#include <SDL/SDL.h>
#include <string>

#include "Definition.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/DefaultResourceProvider.h>

#include "Rendering/Mesh/HitpointBarMesh.h"

#include "Debugger/GlDebugger.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
enum{WINDOW, FULLSCREEN};

class DeferredRenderer;
class Camera;

class SceneOpenGL
{
    public:
	//Constructeur
	static SceneOpenGL* getInstance()
	{
		if(mp_instance == 0)
		{
			mp_instance = new SceneOpenGL("Projet_x", WINDOW_WIDTH, WINDOW_HEIGHT);
			mp_instance->init();
		}
		return mp_instance;
	}

	static void releaseInstance()
	{
		delete mp_instance;
		mp_instance = 0;
	}

	void toggleDisplayMode();
	void setDisplayMode(int i_displayMode);

	//Boucle d'exécution principale
	void mainMenuLoop();
    void gameLoop();

	//Accesseur
	int getWindowWidth(){return m_windowWidth;}
	int getWindowHeight(){return m_windowHeight;}
	SDL_WindowID* getWindow(){return &m_window;}
	bool getClose(){return m_close;}
	Camera* getCamera();

	void setInMainMenu(bool i_inMainMenu){m_inMainMenu = i_inMainMenu;}

	void closeScene(){m_close = true;}

	void initTerrain();

	HitpointBarMesh* getHitpointBarMesh();

    private:
	SceneOpenGL(const std::string& ik_windowTitle, int i_windowWidth, int i_windowHeight);
    ~SceneOpenGL();
	//Instance du singleton
	static SceneOpenGL* mp_instance;

	//Initialisation de la scène
	void init();
	bool initGL();

	//Detection collisions
	void detectCollisions();

	//Paramètre de la fenêtre
    std::string mstr_windowTitle;
    int m_windowWidth;
    int m_windowHeight;

	//Résultat de l'initialisation
    SDL_WindowID m_window;
    SDL_GLContext m_openGLContext;

	SDL_Renderer* m_renderer;

	CEGUI::Window* m_FPSWindow;

	int m_displayMode;

	bool m_close;
	bool m_inMainMenu;

	Camera* mp_camera;

	HitpointBarMesh* mp_hitpointBarMesh;
};

#endif //SCENE_OPENGL_H
