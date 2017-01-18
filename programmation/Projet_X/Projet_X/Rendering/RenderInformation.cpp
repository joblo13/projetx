#include "RenderInformation.h"

RenderInformation::RenderInformation(void* ip_object, RenderInformation* ip_parent)
:mp_parent(ip_parent), m_printID(-1), mp_object(ip_object)
{
	m_baseTransform.loadIdentity();
}



void RenderInformation::setBaseTransform(const Matrix& i_baseTransform)
{
	m_baseTransform = i_baseTransform;
}

Matrix RenderInformation::getModelview()
{
	return m_baseTransform;
}

Matrix RenderInformation::getBaseTransform()
{
	return m_baseTransform;
}

void RenderInformation::setTranslation(const Vector& i_translation)
{
	m_translation = i_translation;
}
const Vector& RenderInformation::getTranslation()
{
	return m_translation;
}

void RenderInformation::setRotation(float i_rotationAngle, const Vector& i_rotationAxies)
{
	m_rotationAngle = i_rotationAngle;
	m_rotationAxies = i_rotationAxies;
}