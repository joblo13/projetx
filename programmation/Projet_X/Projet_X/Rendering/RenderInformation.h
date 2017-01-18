#ifndef RENDER_INFORMATION_H
#define RENDER_INFORMATION_H

#include "AnimationInfo.h"
#include "../Math/Matrix.h"
#include "../Math/Vector.h"

struct RenderInformation
{
	public:
		RenderInformation(void* ip_object, RenderInformation* ip_parent = 0);
		AnimationInfo m_animationInfo;
	
		int m_printID;
		void setBaseTransform(const Matrix& i_baseTransform);
		Matrix getModelview();
		Matrix getBaseTransform();

		void setTranslation(const Vector& i_translation);
		const Vector& getTranslation();
		void setRotation(float i_rotationAngle, const Vector& i_rotationAxies);

		void* getObject(){return mp_object;}
private:
	Matrix m_baseTransform;

	Vector m_translation;
	Vector m_rotationAxies;
	float m_rotationAngle;

	RenderInformation* mp_parent;

	void* mp_object;
};
#endif //RENDER_INFORMATION_H