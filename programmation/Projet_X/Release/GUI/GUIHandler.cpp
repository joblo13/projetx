#include "GUIHandler.h"

#include "../Shaders/Shader.h"
#include "../Rendering/Texture.h"
#include "../Entities/UnitHandler.h"
#include "../Entities/Unit.h"
#include "../Camera/Camera.h"
#include "../Input.h"

#include "FPSGUI.h"
#include "RTSGUI.h"
#include "ConsoleGUI.h"
#include "ShopGUI.h"
#include "MaterialSelectionGUI.h"
#include "EscapeMenuGUI.h"
#include "MainMenuGUI.h"
#include "TemplateEditorGUI.h"
#include "MapGUI.h"


#define TEXTURE_CEGUI 0

GUIHandler* GUIHandler::mp_instance = 0;

GUIHandler::GUIHandler()
:m_needToReload(false)
{
	for(int i(0); i < NB_LAYOUT; ++i)
		mpa_GUI[i] = 0;
}
GUIHandler::~GUIHandler()
{
	for(int i(0); i < NB_LAYOUT; ++i)
	{
		if(mpa_GUI[i] != 0)
			delete mpa_GUI[i];
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool initCEGUI()
///
/// Fonction initialisant le contexte CEGUI
///
/// @return Booléen confirmant que CEGUI à pu etre initialisé
///
////////////////////////////////////////////////////////////////////////
bool GUIHandler::initCEGUI()
{
	Texture::initCEGUITexture();
	try
	{
		mp_openGLRenderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();
	}
	catch(CEGUI::Exception e)
	{
		std::string test = e.what();
		return false;
	}

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	try
	{
	
		CEGUI::DefaultResourceProvider* ressourceProvider = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
		ressourceProvider->setResourceGroupDirectory("schemes", "./GUI/schemes/");
		ressourceProvider->setResourceGroupDirectory("imagesets", "./GUI/imagesets/");
		ressourceProvider->setResourceGroupDirectory("fonts", "./GUI/fonts/");
		ressourceProvider->setResourceGroupDirectory("layouts", "./GUI/layouts/");
		ressourceProvider->setResourceGroupDirectory("looknfeels", "./GUI/looknfeel/");
		ressourceProvider->setResourceGroupDirectory("lua_scripts", "./GUI/lua_scripts/");
		
		CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");
		CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

		CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
		CEGUI::SchemeManager::getSingleton().createFromFile( "Generic.scheme" );
		CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-10.font");

		CEGUI::ImageManager::getSingleton().loadImageset("FPS.imageset", "imagesets");
		CEGUI::ImageManager::getSingleton().loadImageset("Icons.imageset", "imagesets");

		m_root = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "Root.layout", "layouts");
		
		CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow( m_root );
		CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultTooltipType("TaharezLook/Tooltip");

		createGUI();
	}
	catch(CEGUI::Exception e)
	{
		std::string test = e.what();
		return false;
	}
	return true;
}

void GUIHandler::createGUI()
{
	mpa_GUI[FPS_LAYOUT] = new FPSGUI();
	mpa_GUI[RTS_LAYOUT] = new RTSGUI();
	mpa_GUI[CONSOLE_LAYOUT] = new ConsoleGUI();
	mpa_GUI[DEBUG_HUD_LAYOUT] = new DebugHUD();
	mpa_GUI[WEAPONSHOP_LAYOUT] = new ShopGUI();
	mpa_GUI[MATERIAL_SELECTION_LAYOUT] = new MaterialSelectionGUI();
	mpa_GUI[ESCAPE_MENU_LAYOUT] = new EscapeMenuGUI();
	mpa_GUI[MAIN_MENU_LAYOUT] = new MainMenuGUI();
	mpa_GUI[TEMPLATE_EDITOR_LAYOUT] = new TemplateEditorGUI();
	mpa_GUI[MAP_LAYOUT] = new MapGUI();
	
		

	for(int i(0); i < NB_LAYOUT; ++i)
	{
		if(mpa_GUI[i] != 0)
		{
			m_root->addChild(mpa_GUI[i]->getLayout());
			mpa_GUI[i]->registerHandlers();
			mpa_GUI[i]->load();
		}
	}
}

void GUIHandler::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{
	if(m_needToReload)
	{
		reloadLayout();
		m_needToReload = false;
	}
	for(int i(0); i < NB_LAYOUT; ++i)
	{
		if(mpa_GUI[i] != 0)
		{
			if(mpa_GUI[i]->getVisible())
				mpa_GUI[i]->updateLayout(i_timestep, i_teamID, ip_camera);
		}
	}
}

void GUIHandler::preGUIDraw()
{
	Shader::chooseShader(-1);
	Texture::chooseTexture(TEXTURE_CEGUI);
	Texture::bindTexture();
	OpenGLHandler::cullFace(GL_FRONT);
	OpenGLHandler::disable(GL_CULL_FACE);		
	OpenGLHandler::disable(GL_DEPTH_TEST);
}

void GUIHandler::postGUIDraw()
{
		OpenGLHandler::loadState(GL_BLEND);
		OpenGLHandler::loadBlendFunc();
}

void GUIHandler::draw()
{
	try
	{
		preGUIDraw();
		CEGUI::System::getSingleton().renderAllGUIContexts();
		postGUIDraw();



		glBindVertexArray(0);
	}
	catch(CEGUI::Exception e)
	{
		std::string test = e.what();
		assert(false);
	}
}

bool GUIHandler::toggleVisibility(LayoutType i_layoutType)
{
	bool newVisibility(!mpa_GUI[i_layoutType]->getVisible());
	setVisibility(i_layoutType, newVisibility);
	return newVisibility;
}

bool GUIHandler::getVisibility(LayoutType i_layoutType)
{
	if(mpa_GUI[i_layoutType] == 0)
		return false;
	return mpa_GUI[i_layoutType]->getVisible();
}

void GUIHandler::setVisibility(LayoutType i_layoutType, bool i_visibility)
{
	if(mpa_GUI[i_layoutType] != 0)
	{
		mpa_GUI[i_layoutType]->setVisible(i_visibility);
		if(i_visibility)
			mpa_GUI[i_layoutType]->activate();
	}


	//Modification de la visibilité de la souris
	Input* p_input(Input::getInstance());
	for(int i(0); i < NB_LAYOUT; ++i)
	{
		if(mpa_GUI[i]->isMouseVisible())
		{
			p_input->setMouseVisible(true);
			return;
		}
	}
	p_input->setMouseVisible(false);
}


void GUIHandler::printDebugHUD()
{
	static_cast<DebugHUD*>(mpa_GUI[DEBUG_HUD_LAYOUT])->printBuffer();
}

void GUIHandler::reloadLayout()
{
	bool oldVisibility[NB_LAYOUT];
	for(int i(0); i < NB_LAYOUT; ++i)
	{
		oldVisibility[i] = getVisibility((LayoutType)i);
		m_root->removeChild(mpa_GUI[i]->getLayout());
		delete mpa_GUI[i];
	}
	createGUI();

	for(int i(0); i < NB_LAYOUT; ++i)
	{
		setVisibility((LayoutType)i, oldVisibility[i]);
	}
}

void GUIHandler::createOpenGLImage(int i_textureID, LayoutType i_layoutType, const std::string& i_name)
{
	// make the texture
	Texture::chooseTexture(i_textureID);

	Texture::createCEGUITexture(i_layoutType, i_name);

	CEGUI::Window* p_image(mpa_GUI[i_layoutType]->getLayout()->getChild(i_name));
	p_image->setProperty("Image", "RTTImg_" + i_name);
}


void GUIHandler::updateOpenGLImage(int i_textureID, LayoutType i_layoutType, const std::string& i_name, float i_scalling, float i_offsetX, float i_offsetY)
{
	CEGUI::Window* p_image(mpa_GUI[i_layoutType]->getLayout()->getChild(i_name));
	p_image->invalidate();
	// make the texture
	Texture::chooseTexture(i_textureID);
	Texture::updateCEGUITexture(i_layoutType, i_name, i_scalling, i_offsetX, i_offsetY);
}