#include "Timer.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Timer(int i_maxTime)
///
/// Constructeur
/// 
/// @param[in] i_maxTime: Valeur vis�e
///
/// @return Aucun
///
////////////////////////////////////////////////////////////////////////
Timer::Timer(int i_currentTime, int i_maxTime)
:m_currentTime(i_currentTime), m_maxTime(i_maxTime)
{

}
Timer::Timer(const Timer& i_timer)
:m_currentTime(i_timer.m_currentTime), m_maxTime(i_timer.m_maxTime)
{

}
////////////////////////////////////////////////////////////////////////
///
/// @fn ~Timer()
///
/// Destructeur
///
/// @return Aucun
///
////////////////////////////////////////////////////////////////////////
Timer::~Timer()
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool doAction()
///
/// Fonction permettant de determiner si on peut effectuer l'action et d�cr�mentant le compteur si c'est le cas
///
/// @param[in] i_reset: On reset le timer si l'on peut faire l'action et que le bool est � vrai
///
/// @return Bool�en confirmant l'atteinte de la limite du timer
///
////////////////////////////////////////////////////////////////////////
bool Timer::doAction(bool i_reset)
{
	//On v�rifie si le temps est atteint
	if(m_maxTime <= m_currentTime)
	{
		//Si il est atteint, on remets le compteur � z�ro et on retourne vrai
		if(i_reset)
			m_currentTime = 0;
		return true;
	}
	else
	{		
		//Sinon, on retourne faux
		return false;
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void increment(int i_increment)
///
/// Fonction permettant d'incrementer le compteur
/// 
/// @param[in] i_increment: Valeur � ajouter au compteur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Timer::increment(int i_increment)
{
	//On ajoute l'increment
	m_currentTime += i_increment;

	//Si la valeur d�passe le maximum, on met le temps courant au maximum
	if(m_maxTime <= m_currentTime)
		m_currentTime = m_maxTime;
	
}