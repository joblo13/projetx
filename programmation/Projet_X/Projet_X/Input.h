#ifndef INPUT_H
#define INPUT_H

#include <SDL/SDL.h>
#include <set>
#include <list>
#include <string>
#include <CEGUI/CEGUI.h>
#include <CEGUI/InjectedInputReceiver.h>

#include "Camera/Camera.h"

enum InputState{FPS_INPUT, CONSOLE_INPUT, RTS_INPUT, MAP_INPUT};
enum MouseButton{MOUSE_LEFT, MOUSE_MIDDLE, MOUSE_RIGHT};

class Observer;
class Camera;

class Input
{
    public:
		//Singleton
		static Input* getInstance()
		{
			if(mp_instance == 0)
				mp_instance = new Input();
			return mp_instance;
		}

		static void releaseInstance()
		{
			delete mp_instance;
			mp_instance = 0;
		}

		static std::string keyCodeToString(SDLKey i_key);

	//Récupérateur d'évènement
    void updateEvent();

	void changeCameraType(Camera*& ip_camera);

	//Gestion des observateurs
	void attachObserver(Observer* ip_observer);
	void removeObserver(Observer* ip_observer);
	void notifyObservers();

	//Modifieur

	//Accesseur
	InputState getInputState();

	bool getKeyState(const std::string& ik_name){return ma_keystates[ma_keyconf[ik_name]];}
	bool getKeyPress(const std::string& ik_name){return ma_keystates[ma_keyconf[ik_name]] && !ma_lastKeystates[ma_keyconf[ik_name]];}
	bool getKeyRelease(const std::string& ik_name){return !ma_keystates[ma_keyconf[ik_name]] && ma_lastKeystates[ma_keyconf[ik_name]];}

	bool getMouseButState(MouseButton i_buttonNum)const{return ma_mouseButtons[i_buttonNum];}
	bool getMouseButPress(MouseButton i_buttonNum)const{return ma_mouseButtons[i_buttonNum] && !ma_lastMouseButtons[i_buttonNum];}
	bool getMouseButRelease(MouseButton i_buttonNum)const{return !ma_mouseButtons[i_buttonNum] && ma_lastMouseButtons[i_buttonNum];}

	int getMousePosX()const{return m_mouseX;}
	int getMousePosY()const{return m_mouseY;}

	void setMouseVisible(bool i_isMouseVisible);
	bool getMouseVisible(){return m_isMouseVisible;}

	//Modification de la souris
	void centerMouse();

	const std::set<SDLKey>& getKeyList(){return m_keyList;}

	
	SDLKey getSDLKey(const std::string& ik_name)
	{
		return ma_keyconf[ik_name];}

	bool getSDLKeystate(SDLKey i_SDLKey)
	{
		return ma_keystates[i_SDLKey];
	}

	void addKey(SDLKey i_SDLKey){ma_keystates.insert(std::pair<SDLKey, bool>(i_SDLKey, false)); ma_lastKeystates.insert(std::pair<SDLKey, bool>(i_SDLKey, false));}

	void setCameraType(CameraType i_cameraType){m_cameraType = i_cameraType;}

    private:
	//Singleton
	Input();
	~Input();
	static Input* mp_instance;

	CEGUI::Key::Scan SDLKeyToCEGUIKey(SDLKey key);
	int generateUnicode(SDLKey key);

	//Fonction gérant les événements
    virtual void OnMouseMotion(const SDL_Event& ik_event);
    virtual void OnMouseButton(const SDL_Event& ik_event);
    virtual void OnKeyboard(const SDL_Event& ik_event, SDL_EventType i_eventType);

    //Clavier
	//État des touches
    typedef std::map<SDLKey,bool> KeyStates;
    KeyStates ma_keystates;
    KeyStates ma_lastKeystates;

	//Liste des touches utilisées
	typedef std::map<std::string,SDLKey> KeyConf;
    KeyConf ma_keyconf;

	//Souris
	bool m_isMouseVisible;
    int m_mouseX;
    int m_mouseY;
    bool ma_mouseButtons[8];
	bool ma_lastMouseButtons[8];

	//Observeur
	std::list<Observer*> m_observers;

	std::set<SDLKey> m_keyList;

	CameraType m_cameraType;
};

#endif //INPUT_H
