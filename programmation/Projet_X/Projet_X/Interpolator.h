#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <SDL/SDL.h>
#include <algorithm>

class Interpolator
{
	public:
		Interpolator(float* i_valueToModify, float i_iterpolationSpeed);
		Interpolator(const Interpolator& i_interpolator);
		~Interpolator();

		void interpolate(Uint32 i_timestep);
		void addToTargetValue(float i_toAdd){m_extraTargetValue += i_toAdd;}

		float getExtraValue(){return m_extraValue;}

		void setvalueToModify(float* ip_valueToModify){m_valueToModify = ip_valueToModify;}
		void setExtraTargetValue(float i_extraTargetValue){m_extraTargetValue = i_extraTargetValue;}
		void setInterpolationSpeed(float i_interpolationSpeed){m_interpolationSpeed = i_interpolationSpeed;}

		void reset();

	private:
		float m_interpolationSpeed;

		float* m_valueToModify;

		float m_extraValue;
		float m_extraTargetValue;

};

#endif //INTERPOLATOR_H
