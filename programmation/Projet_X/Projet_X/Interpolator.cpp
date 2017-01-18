#include "Interpolator.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Interpolator(float* i_valueToModify)
///
/// Constructeur
/// 
/// @param[in] i_valueToModify: Valeur à interpoler
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Interpolator::Interpolator(float* i_valueToModify, float i_iterpolationSpeed)
:m_valueToModify(i_valueToModify), m_extraValue(0), m_extraTargetValue(0), m_interpolationSpeed(i_iterpolationSpeed)
{

}
Interpolator::Interpolator(const Interpolator& i_interpolator)
:m_valueToModify(i_interpolator.m_valueToModify), 
m_extraValue(i_interpolator.m_extraValue), m_extraTargetValue(i_interpolator.m_extraTargetValue), 
m_interpolationSpeed(i_interpolator.m_interpolationSpeed)
{

}
////////////////////////////////////////////////////////////////////////
///
/// @fn ~Interpolator()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Interpolator::~Interpolator()
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn void interpolate(Uint32 i_timestep)
///
/// Fonction permettant d'interpoler
/// 
/// @param[in] i_timestep: Nombre de pas de la dernière loop
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Interpolator::interpolate(Uint32 i_timestep)
{
	float toTransfer((m_extraTargetValue < m_extraValue?-1:1) * std::min(abs(abs(m_extraTargetValue) - abs(m_extraValue)), (i_timestep * m_interpolationSpeed)));

	*m_valueToModify += toTransfer;
	m_extraValue += toTransfer;
}

void Interpolator::reset()
{
	m_extraValue = 0;
	m_extraTargetValue = 0;
}