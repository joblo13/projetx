#include "SceneOpenGL.h"
//#include "vld.h" // Leak monitor (chute de FPS et impossibilité d'exit le programme)

int main(int argc, char *argv[])
{
    // Création de la scène
    SceneOpenGL* p_scene(SceneOpenGL::getInstance());
	p_scene->mainMenuLoop();
	p_scene->initTerrain();
	p_scene->gameLoop();

    return 0;
}
