#include "Input.h"

#include "SceneOpenGL.h"
#include "Observer.h"
#include "Camera/CameraRTS.h"
#include "GUI/GUIHandler.h"
#include "GUI/MapGUI.h"

//Initialisation de l'instance
Input* Input::mp_instance = 0;

////////////////////////////////////////////////////////////////////////
///
/// @fn Input()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Input::Input() 
:m_mouseX(0), m_mouseY(0), m_isMouseVisible(true), m_cameraType(FPS_CAMERA)
{
	//SDL_GrabMode test = SDL_WM_GrabInput(SDL_GRAB_ON);
    // Initialisation de la map du clavier
    ma_keyconf["escape"] = SDLK_ESCAPE;
    ma_keyconf["forward"] = SDLK_w;
    ma_keyconf["backward"] = SDLK_s;
    ma_keyconf["strafe_left"] = SDLK_a;
    ma_keyconf["strafe_right"] = SDLK_d;
    ma_keyconf["boost"] = SDLK_LSHIFT;
	ma_keyconf["jump"] = SDLK_SPACE;
	ma_keyconf["changeView"] = SDLK_TAB;
	ma_keyconf["reload"] = SDLK_r;	
	ma_keyconf["weaponShop"] = SDLK_i;
	ma_keyconf["wireMode"] = SDLK_p;
	ma_keyconf["thirdGunFunction"] = SDLK_f;
	ma_keyconf["windowMode"] = SDLK_F12;
	ma_keyconf["materialSelection"] = SDLK_h;

	ma_keyconf["addRTSAction"] = SDLK_LSHIFT;


	ma_keyconf["verticalUp"] = SDLK_PAGEUP;
	ma_keyconf["verticalDown"] = SDLK_PAGEDOWN;

	ma_keyconf["zoomIn"] = SDLK_PAGEUP;
	ma_keyconf["zoomOut"] = SDLK_PAGEDOWN;

	ma_keyconf["up"] = SDLK_UP;
	ma_keyconf["down"] = SDLK_DOWN;
	ma_keyconf["left"] = SDLK_LEFT;
	ma_keyconf["right"] = SDLK_RIGHT;


	ma_keyconf["switchGun1"] = SDLK_1;
	ma_keyconf["switchGun2"] = SDLK_2;
	ma_keyconf["switchGun3"] = SDLK_3;
	ma_keyconf["switchGun4"] = SDLK_4;
	ma_keyconf["switchGun5"] = SDLK_5;
	ma_keyconf["switchGun6"] = SDLK_6;
	ma_keyconf["switchGun7"] = SDLK_7;
	ma_keyconf["switchGun8"] = SDLK_8;
	ma_keyconf["switchGun9"] = SDLK_9;
	ma_keyconf["switchGun0"] = SDLK_0;

	ma_keyconf["ZONE_SELECTION_FILL"] = SDLK_q;
	ma_keyconf["ZONE_SELECTION_TEMPLATE"] = SDLK_e;
	ma_keyconf["UPGRADE_TEMPLATE"] = SDLK_q;
	

	ma_keyconf["console"] = SDLK_F1;

	//Initialisation des états du clavier

	for(KeyConf::iterator it(ma_keyconf.begin()), keyConfEnd(ma_keyconf.end()); it != keyConfEnd; ++it)
	{
		ma_keystates[it->second] = false;
		ma_lastKeystates[it->second] = false;
	}
	ma_keystates[SDLK_LSHIFT] = false;
	ma_lastKeystates[SDLK_LSHIFT] = false;

    // Initialisation du tableau boutonsSouris
    for(int i(0); i < 8; ++i)
	{
        ma_mouseButtons[i] = false;
		ma_lastMouseButtons[i] = false;
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Input()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Input::~Input()
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn void OnMouseMotion(const SDL_Event& ik_event)
///
/// Fonction traitant les évènement lancé lorsque l'on bouge la souris
/// 
/// @param[in] ik_event : Évenement SDL
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::OnMouseMotion(const SDL_Event& ik_event)
{
	//On enregistre la position actuel du clavier
    m_mouseX = ik_event.motion.x;
    m_mouseY = ik_event.motion.y;

	notifyObservers();

	if(m_isMouseVisible)
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition((float)m_mouseX, (float)m_mouseY);

}

////////////////////////////////////////////////////////////////////////
///
/// @fn void OnMouseButton(const SDL_Event& ik_event)
///
/// Fonction traitant les évènement lancé lorsque l'on éffectue un clique de souris
/// 
/// @param[in] ik_event : Évenement SDL
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::OnMouseButton(const SDL_Event& ik_event)
{

	//On enregistre la pression du bouton de droit
	if(SDL_BUTTON_LEFT == ik_event.button.button)
		ma_mouseButtons[SDL_BUTTON_LEFT-1] = !ma_mouseButtons[SDL_BUTTON_LEFT-1];
	//On enregistre la pression du bouton de gauche
	if(SDL_BUTTON_RIGHT == ik_event.button.button)
		ma_mouseButtons[SDL_BUTTON_RIGHT-1] = !ma_mouseButtons[SDL_BUTTON_RIGHT-1];

	if(SDL_BUTTON_MIDDLE == ik_event.button.button)
		ma_mouseButtons[SDL_BUTTON_MIDDLE-1] = !ma_mouseButtons[SDL_BUTTON_MIDDLE-1];

	if(m_isMouseVisible)
	{
		if(ma_lastMouseButtons[SDL_BUTTON_LEFT-1] != ma_mouseButtons[SDL_BUTTON_LEFT-1])
		{
			if(ma_mouseButtons[SDL_BUTTON_LEFT-1])
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
			else
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);
		}
		if(ma_lastMouseButtons[SDL_BUTTON_RIGHT-1] != ma_mouseButtons[SDL_BUTTON_RIGHT-1])
		{
			if(ma_mouseButtons[SDL_BUTTON_RIGHT-1])
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::RightButton);
			else
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::RightButton);
		}
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void OnKeyboard(const SDL_Event& ik_event)
///
/// Évenement lancé lorsque effectue l'on appui sur une touche du clavier
/// 
/// @param[in] ik_event : Évenement SDL
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::OnKeyboard(const SDL_Event& ik_event, SDL_EventType i_eventType)
{
	//On itère sur tout les touches gérées par le jeu
    for (KeyStates::iterator iterx_iter(ma_keystates.begin()), keyStatesEnd(ma_keystates.end());iterx_iter != keyStatesEnd; ++iterx_iter)
    {
		//Lorsque l'on trouve la touche modifier dans la liste des touches valides
        if (ik_event.key.keysym.sym == iterx_iter->first)
        {
			//On met à jour la touche
            iterx_iter->second = (ik_event.type == SDL_KEYDOWN);
            break;
        }
    }
	if(i_eventType == SDL_KEYDOWN)
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(SDLKeyToCEGUIKey(ik_event.key.keysym.sym));
		CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(generateUnicode(ik_event.key.keysym.sym));
		m_keyList.insert(ik_event.key.keysym.sym);
	}
	else if(i_eventType == SDL_KEYUP)
	{
		m_keyList.erase(ik_event.key.keysym.sym);
	}


	GUIHandler* p_GUIHandler(GUIHandler::getInstance());

	if(getKeyPress("console"))
	{
		p_GUIHandler->toggleVisibility(CONSOLE_LAYOUT);
	}
	if(getKeyPress("escape") && getInputState() != CONSOLE_INPUT)
		p_GUIHandler->toggleVisibility(ESCAPE_MENU_LAYOUT);

	if(getKeyPress("windowMode") && getInputState() != CONSOLE_INPUT)
		SceneOpenGL::getInstance()->toggleDisplayMode();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void centerMouse()
///
/// Fonction permettant de centrer la souris
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::centerMouse()
{
	//On trouve le centre de l'écran
	SDL_Window* p_window(*SceneOpenGL::getInstance()->getWindow());
	int centerX, centerY;
	SDL_GetWindowSize(p_window, &centerX, &centerY);
	centerX /= 2;
	centerY /= 2;

	//On replace le curseur au centre de la fenêtre
	SDL_WarpMouseInWindow(p_window, centerX, centerY);
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void updateEvent()
///
/// Fonction s'occupant de la gestion des évènements
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::updateEvent()
{
     // Variable SDL_Event (en statique pour rester en mémoire)

    static SDL_Event events;


    // Pour éviter de fausses valeurs au niveau de la souris, on réinitialise certains attributs


    ma_mouseButtons[SDL_BUTTON_WHEELDOWN] = false;
    ma_mouseButtons[SDL_BUTTON_WHEELUP] = false;

	ma_lastKeystates = ma_keystates;
	for(int i(0); i < 8; ++i)
		ma_lastMouseButtons[i] = ma_mouseButtons[i];

    // Mise à jour des évènements
    while(SDL_PollEvent(&events))
    {
        // On teste les autres évènements

        switch(events.type)
        {
            // Cas de pression sur une touche

            case SDL_KEYDOWN:
                OnKeyboard(events, SDL_KEYDOWN);
            break;


            // Cas du relâchement d'une touche

            case SDL_KEYUP:
                OnKeyboard(events, SDL_KEYUP);
            break;


            // Cas d'un mouvement de souris

            case SDL_MOUSEMOTION:
                OnMouseMotion(events);
            break;


            // Cas de pression sur un bouton de souris

            case SDL_MOUSEBUTTONDOWN:
                OnMouseButton(events);
            break;


            // Cas du relâchement d'un bouton de souris

            case SDL_MOUSEBUTTONUP:

                if(events.button.button != SDL_BUTTON_WHEELDOWN && events.button.button != SDL_BUTTON_WHEELUP)
                    ma_mouseButtons[events.button.button] = false;
                OnMouseButton(events);
            break;


            // Cas de la fermeture de la fenêtre

            case SDL_WINDOWEVENT:

                if(events.window.event == SDL_WINDOWEVENT_CLOSE)
                    SceneOpenGL::getInstance()->closeScene();
            break;

            // Défaut

            default:
            break;
        }
    }
}

void Input::changeCameraType(Camera*& ip_camera)
{
	Camera* p_tempCamera(0);
	switch(m_cameraType)
	{
		case FPS_CAMERA:
			if(ip_camera->getCameraType() != FPS_CAMERA)
			{
				if(ip_camera->isAUnitSelected())
				{
					p_tempCamera = new Camera(*ip_camera);
					delete ip_camera;
					ip_camera = p_tempCamera;
				}
			}
			break;
		case RTS_CAMERA:
			if(ip_camera->getCameraType() != RTS_CAMERA)
			{
				p_tempCamera = new CameraRTS(*ip_camera);
				delete ip_camera;
				ip_camera = p_tempCamera;
			}
			break;
	}
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void attachObserver(Observer* ip_observer)
///
/// Fonction permettant d'ajouter un observeur d'input
/// 
/// @param[in] ip_observer: observeur à ajouter
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::attachObserver(Observer* ip_observer)
{
	//On ajoute l'observeur à la liste
	m_observers.push_back(ip_observer);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void removeObserver(Observer* ipx_observer)
///
/// Fonction permettant de retirer un observeur de la liste
/// 
/// @param[in] ipx_observer: observeur à retirer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::removeObserver(Observer* ip_observer)
{
	//On le retire de la liste d'observateur s'il est présent
	for(std::list<Observer*>::iterator iter(m_observers.begin()); iter != m_observers.end(); ++iter)
	{
		if(*iter == ip_observer)
		{
			m_observers.erase(iter);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void notifyObservers()
///
/// Fonction permettant d'avertir tout les observeurs
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Input::notifyObservers()
{
	//On avertie tout les obervers
	for(std::list<Observer*>::iterator iter = m_observers.begin(); iter != m_observers.end(); ++iter)
	{
		(*iter)->update();
	}
}

void Input::setMouseVisible(bool i_isMouseVisible)
{
	if(m_isMouseVisible != i_isMouseVisible)
	{
		m_isMouseVisible = i_isMouseVisible;

		SDL_ShowCursor(m_isMouseVisible);

		if(!m_isMouseVisible)
			centerMouse();
	}
}

InputState Input::getInputState()
{
	Camera* p_camera(SceneOpenGL::getInstance()->getCamera());
	GUIHandler* p_guiHandler(GUIHandler::getInstance());

	if(p_guiHandler->getLayout(CONSOLE_LAYOUT)->getVisible())
		return CONSOLE_INPUT;
	if(((MapGUI*)p_guiHandler->getLayout(MAP_LAYOUT))->isMouseInside())
		return MAP_INPUT;

	switch(p_camera->getCameraType())
	{
		case FPS_CAMERA:
			return FPS_INPUT;
		case RTS_CAMERA:
			return RTS_INPUT;
	}
}

int Input::generateUnicode(SDLKey key)
{
	 switch (key)
     {
		 case SDLK_SPACE:        return 32;
		 case SDLK_COMMA:        return 44;
		 case SDLK_MINUS:        return 45;
		 case SDLK_PERIOD:       return 46;
		 case SDLK_SLASH: case 1073741908:		return 47;
		 case SDLK_KP0: case SDLK_0:            return 48;
		 case SDLK_KP1: case SDLK_1:            return 49;
		 case SDLK_KP2: case SDLK_2:            return 50;
		 case SDLK_KP3: case SDLK_3:            return 51;
		 case SDLK_KP4: case SDLK_4:            return 52;
		 case SDLK_KP5: case SDLK_5:            return 53;
		 case SDLK_KP6: case SDLK_6:            return 54;
		 case SDLK_KP7: case SDLK_7:            return 55;
		 case SDLK_KP8: case SDLK_8:            return 56;
		 case SDLK_KP9: case SDLK_9:            return 57;
		 case SDLK_COLON:        return 58;
		 case SDLK_SEMICOLON:    return 59;
		 case SDLK_EQUALS:       return 61;
		 case SDLK_LEFTBRACKET:  return 123;
		 case SDLK_BACKSLASH:    return 92;
		 case SDLK_RIGHTBRACKET: return 125;
		 case SDLK_a:            return ma_keystates[SDLK_LSHIFT]?65:97;
		 case SDLK_b:            return ma_keystates[SDLK_LSHIFT]?66:98;
		 case SDLK_c:            return ma_keystates[SDLK_LSHIFT]?67:99;
		 case SDLK_d:            return ma_keystates[SDLK_LSHIFT]?68:100;
		 case SDLK_e:            return ma_keystates[SDLK_LSHIFT]?69:101;
		 case SDLK_f:            return ma_keystates[SDLK_LSHIFT]?70:102;
		 case SDLK_g:            return ma_keystates[SDLK_LSHIFT]?71:103;
		 case SDLK_h:            return ma_keystates[SDLK_LSHIFT]?72:104;
		 case SDLK_i:            return ma_keystates[SDLK_LSHIFT]?73:105;
		 case SDLK_j:            return ma_keystates[SDLK_LSHIFT]?74:106;
		 case SDLK_k:            return ma_keystates[SDLK_LSHIFT]?75:107;
		 case SDLK_l:            return ma_keystates[SDLK_LSHIFT]?76:108;
		 case SDLK_m:            return ma_keystates[SDLK_LSHIFT]?77:109;
		 case SDLK_n:            return ma_keystates[SDLK_LSHIFT]?78:110;
		 case SDLK_o:            return ma_keystates[SDLK_LSHIFT]?79:111;
		 case SDLK_p:            return ma_keystates[SDLK_LSHIFT]?80:112;
		 case SDLK_q:            return ma_keystates[SDLK_LSHIFT]?81:113;
		 case SDLK_r:            return ma_keystates[SDLK_LSHIFT]?82:114;
		 case SDLK_s:            return ma_keystates[SDLK_LSHIFT]?83:115;
		 case SDLK_t:            return ma_keystates[SDLK_LSHIFT]?84:116;
		 case SDLK_u:            return ma_keystates[SDLK_LSHIFT]?85:117;
		 case SDLK_v:            return ma_keystates[SDLK_LSHIFT]?86:118;
		 case SDLK_w:            return ma_keystates[SDLK_LSHIFT]?87:119;
		 case SDLK_x:            return ma_keystates[SDLK_LSHIFT]?88:120;
		 case SDLK_y:            return ma_keystates[SDLK_LSHIFT]?89:121;
		 case SDLK_z:            return ma_keystates[SDLK_LSHIFT]?90:122;
     }
	 return 0;
}
CEGUI::Key::Scan Input::SDLKeyToCEGUIKey(SDLKey key)
 {
     switch (key)
     {
		 case SDLK_BACKSPACE:    return CEGUI::Key::Backspace;
		 case SDLK_TAB:          return CEGUI::Key::Tab;
		 case SDLK_RETURN:       return CEGUI::Key::Return;
		 case SDLK_PAUSE:        return CEGUI::Key::Pause;
		 case SDLK_ESCAPE:       return CEGUI::Key::Escape;
		 case SDLK_SPACE:        return CEGUI::Key::Space;
		 case SDLK_COMMA:        return CEGUI::Key::Comma;
		 case SDLK_MINUS:        return CEGUI::Key::Minus;
		 case SDLK_PERIOD:       return CEGUI::Key::Period;
		 case SDLK_SLASH:        return CEGUI::Key::Slash;
		 case SDLK_0:            return CEGUI::Key::Zero;
		 case SDLK_1:            return CEGUI::Key::One;
		 case SDLK_2:            return CEGUI::Key::Two;
		 case SDLK_3:            return CEGUI::Key::Three;
		 case SDLK_4:            return CEGUI::Key::Four;
		 case SDLK_5:            return CEGUI::Key::Five;
		 case SDLK_6:            return CEGUI::Key::Six;
		 case SDLK_7:            return CEGUI::Key::Seven;
		 case SDLK_8:            return CEGUI::Key::Eight;
		 case SDLK_9:            return CEGUI::Key::Nine;
		 case SDLK_COLON:        return CEGUI::Key::Colon;
		 case SDLK_SEMICOLON:    return CEGUI::Key::Semicolon;
		 case SDLK_EQUALS:       return CEGUI::Key::Equals;
		 case SDLK_LEFTBRACKET:  return CEGUI::Key::LeftBracket;
		 case SDLK_BACKSLASH:    return CEGUI::Key::Backslash;
		 case SDLK_RIGHTBRACKET: return CEGUI::Key::RightBracket;
		 case SDLK_a:            return CEGUI::Key::A;
		 case SDLK_b:            return CEGUI::Key::B;
		 case SDLK_c:            return CEGUI::Key::C;
		 case SDLK_d:            return CEGUI::Key::D;
		 case SDLK_e:            return CEGUI::Key::E;
		 case SDLK_f:            return CEGUI::Key::F;
		 case SDLK_g:            return CEGUI::Key::G;
		 case SDLK_h:            return CEGUI::Key::H;
		 case SDLK_i:            return CEGUI::Key::I;
		 case SDLK_j:            return CEGUI::Key::J;
		 case SDLK_k:            return CEGUI::Key::K;
		 case SDLK_l:            return CEGUI::Key::L;
		 case SDLK_m:            return CEGUI::Key::M;
		 case SDLK_n:            return CEGUI::Key::N;
		 case SDLK_o:            return CEGUI::Key::O;
		 case SDLK_p:            return CEGUI::Key::P;
		 case SDLK_q:            return CEGUI::Key::Q;
		 case SDLK_r:            return CEGUI::Key::R;
		 case SDLK_s:            return CEGUI::Key::S;
		 case SDLK_t:            return CEGUI::Key::T;
		 case SDLK_u:            return CEGUI::Key::U;
		 case SDLK_v:            return CEGUI::Key::V;
		 case SDLK_w:            return CEGUI::Key::W;
		 case SDLK_x:            return CEGUI::Key::X;
		 case SDLK_y:            return CEGUI::Key::Y;
		 case SDLK_z:            return CEGUI::Key::Z;
		 case SDLK_DELETE:       return CEGUI::Key::Delete;
		 case SDLK_KP0:          return CEGUI::Key::Numpad0;
		 case SDLK_KP1:          return CEGUI::Key::Numpad1;
		 case SDLK_KP2:          return CEGUI::Key::Numpad2;
		 case SDLK_KP3:          return CEGUI::Key::Numpad3;
		 case SDLK_KP4:          return CEGUI::Key::Numpad4;
		 case SDLK_KP5:          return CEGUI::Key::Numpad5;
		 case SDLK_KP6:          return CEGUI::Key::Numpad6;
		 case SDLK_KP7:          return CEGUI::Key::Numpad7;
		 case SDLK_KP8:          return CEGUI::Key::Numpad8;
		 case SDLK_KP9:          return CEGUI::Key::Numpad9;
		 case SDLK_KP_PERIOD:    return CEGUI::Key::Decimal;
		 case SDLK_KP_DIVIDE:    return CEGUI::Key::Divide;
		 case SDLK_KP_MULTIPLY:  return CEGUI::Key::Multiply;
		 case SDLK_KP_MINUS:     return CEGUI::Key::Subtract;
		 case SDLK_KP_PLUS:      return CEGUI::Key::Add;
		 case SDLK_KP_ENTER:     return CEGUI::Key::NumpadEnter;
		 case SDLK_KP_EQUALS:    return CEGUI::Key::NumpadEquals;
		 case SDLK_UP:           return CEGUI::Key::ArrowUp;
		 case SDLK_DOWN:         return CEGUI::Key::ArrowDown;
		 case SDLK_RIGHT:        return CEGUI::Key::ArrowRight;
		 case SDLK_LEFT:         return CEGUI::Key::ArrowLeft;
		 case SDLK_INSERT:       return CEGUI::Key::Insert;
		 case SDLK_HOME:         return CEGUI::Key::Home;
		 case SDLK_END:          return CEGUI::Key::End;
		 case SDLK_PAGEUP:       return CEGUI::Key::PageUp;
		 case SDLK_PAGEDOWN:     return CEGUI::Key::PageDown;
		 case SDLK_F1:           return CEGUI::Key::F1;
		 case SDLK_F2:           return CEGUI::Key::F2;
		 case SDLK_F3:           return CEGUI::Key::F3;
		 case SDLK_F4:           return CEGUI::Key::F4;
		 case SDLK_F5:           return CEGUI::Key::F5;
		 case SDLK_F6:           return CEGUI::Key::F6;
		 case SDLK_F7:           return CEGUI::Key::F7;
		 case SDLK_F8:           return CEGUI::Key::F8;
		 case SDLK_F9:           return CEGUI::Key::F9;
		 case SDLK_F10:          return CEGUI::Key::F10;
		 case SDLK_F11:          return CEGUI::Key::F11;
		 case SDLK_F12:          return CEGUI::Key::F12;
		 case SDLK_F13:          return CEGUI::Key::F13;
		 case SDLK_F14:          return CEGUI::Key::F14;
		 case SDLK_F15:          return CEGUI::Key::F15;
		 case SDLK_NUMLOCK:      return CEGUI::Key::NumLock;
		 case SDLK_SCROLLOCK:    return CEGUI::Key::ScrollLock;
		 case SDLK_RSHIFT:       return CEGUI::Key::RightShift;
		 case SDLK_LSHIFT:       return CEGUI::Key::LeftShift;
		 case SDLK_RCTRL:        return CEGUI::Key::RightControl;
		 case SDLK_LCTRL:        return CEGUI::Key::LeftControl;
		 case SDLK_RALT:         return CEGUI::Key::RightAlt;
		 case SDLK_LALT:         return CEGUI::Key::LeftAlt;
		 case SDLK_LSUPER:       return CEGUI::Key::LeftWindows;
		 case SDLK_RSUPER:       return CEGUI::Key::RightWindows;
		 case SDLK_SYSREQ:       return CEGUI::Key::SysRq;
		 case SDLK_MENU:         return CEGUI::Key::AppMenu;
		 case SDLK_POWER:        return CEGUI::Key::Power;

     }
 }

 std::string Input::keyCodeToString(SDLKey i_key)
 {
	      switch (i_key)
     {
		 case SDLK_BACKSPACE:    return "Backspace";
		 case SDLK_TAB:          return "Tab";
		 case SDLK_RETURN:       return "Return";
		 case SDLK_PAUSE:        return "Pause";
		 case SDLK_ESCAPE:       return "Escape";
		 case SDLK_SPACE:        return "Space";
		 case SDLK_COMMA:        return ",";
		 case SDLK_MINUS:        return "-";
		 case SDLK_PERIOD:       return ".";
		 case SDLK_SLASH:        return "/";
		 case SDLK_0:            return "0";
		 case SDLK_1:            return "1";
		 case SDLK_2:            return "2";
		 case SDLK_3:            return "3";
		 case SDLK_4:            return "4";
		 case SDLK_5:            return "5";
		 case SDLK_6:            return "6";
		 case SDLK_7:            return "7";
		 case SDLK_8:            return "8";
		 case SDLK_9:            return "9";
		 case SDLK_COLON:        return ":";
		 case SDLK_SEMICOLON:    return ";";
		 case SDLK_EQUALS:       return "=";
		 case SDLK_LEFTBRACKET:  return "[";
		 case SDLK_BACKSLASH:    return "\\";
		 case SDLK_RIGHTBRACKET: return "]";
		 case SDLK_a:            return "A";
		 case SDLK_b:            return "B";
		 case SDLK_c:            return "C";
		 case SDLK_d:            return "D";
		 case SDLK_e:            return "E";
		 case SDLK_f:            return "F";
		 case SDLK_g:            return "G";
		 case SDLK_h:            return "H";
		 case SDLK_i:            return "I";
		 case SDLK_j:            return "J";
		 case SDLK_k:            return "K";
		 case SDLK_l:            return "L";
		 case SDLK_m:            return "M";
		 case SDLK_n:            return "N";
		 case SDLK_o:            return "O";
		 case SDLK_p:            return "P";
		 case SDLK_q:            return "Q";
		 case SDLK_r:            return "R";
		 case SDLK_s:            return "S";
		 case SDLK_t:            return "T";
		 case SDLK_u:            return "U";
		 case SDLK_v:            return "V";
		 case SDLK_w:            return "W";
		 case SDLK_x:            return "X";
		 case SDLK_y:            return "Y";
		 case SDLK_z:            return "Z";
		 case SDLK_DELETE:       return "Delete";
		 case SDLK_KP0:          return "Numpad0";
		 case SDLK_KP1:          return "Numpad1";
		 case SDLK_KP2:          return "Numpad2";
		 case SDLK_KP3:          return "Numpad3";
		 case SDLK_KP4:          return "Numpad4";
		 case SDLK_KP5:          return "Numpad5";
		 case SDLK_KP6:          return "Numpad6";
		 case SDLK_KP7:          return "Numpad7";
		 case SDLK_KP8:          return "Numpad8";
		 case SDLK_KP9:          return "Numpad9";
		 case SDLK_KP_PERIOD:    return "Numpad.";
		 case SDLK_KP_DIVIDE:    return "Numpad/";
		 case SDLK_KP_MULTIPLY:  return "Numpad*";
		 case SDLK_KP_MINUS:     return "Numpad-";
		 case SDLK_KP_PLUS:      return "Numpad+";
		 case SDLK_KP_ENTER:     return "Numpad Enter";
		 case SDLK_KP_EQUALS:    return "Numpad=";
		 case SDLK_UP:           return "Arrow Up";
		 case SDLK_DOWN:         return "Arrow Down";
		 case SDLK_RIGHT:        return "Arrow Right";
		 case SDLK_LEFT:         return "Arrow Left";
		 case SDLK_INSERT:       return "Insert";
		 case SDLK_HOME:         return "Home";
		 case SDLK_END:          return "End";
		 case SDLK_PAGEUP:       return "PageUp";
		 case SDLK_PAGEDOWN:     return "PageDown";
		 case SDLK_F1:           return "F1";
		 case SDLK_F2:           return "F2";
		 case SDLK_F3:           return "F3";
		 case SDLK_F4:           return "F4";
		 case SDLK_F5:           return "F5";
		 case SDLK_F6:           return "F6";
		 case SDLK_F7:           return "F7";
		 case SDLK_F8:           return "F8";
		 case SDLK_F9:           return "F9";
		 case SDLK_F10:          return "F10";
		 case SDLK_F11:          return "F11";
		 case SDLK_F12:          return "F12";
		 case SDLK_F13:          return "F13";
		 case SDLK_F14:          return "F14";
		 case SDLK_F15:          return "F15";
		 case SDLK_NUMLOCK:      return "NumLock";
		 case SDLK_SCROLLOCK:    return "ScrollLock";
		 case SDLK_RSHIFT:       return "R Shift";
		 case SDLK_LSHIFT:       return "L Shift";
		 case SDLK_RCTRL:        return "R Control";
		 case SDLK_LCTRL:        return "L Control";
		 case SDLK_RALT:         return "R Alt";
		 case SDLK_LALT:         return "L Alt";
		 case SDLK_LSUPER:       return "L Windows";
		 case SDLK_RSUPER:       return "R Windows";
		 case SDLK_SYSREQ:       return "Sys Rq";
		 case SDLK_MENU:         return "App Menu";
		 case SDLK_POWER:        return "Power";

     }
 }