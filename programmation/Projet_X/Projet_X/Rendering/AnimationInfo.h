#ifndef ANIMATION_INFO_H
#define ANIMATION_INFO_H

#include <vector>
#include <tuple>
#include "../Math/Matrix.h"

struct AnimationInfo
{
public:
	AnimationInfo();
	~AnimationInfo();

	std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> m_lastPositions;
	float m_lastTime;

	int m_currentAnimation;
	int m_currentFrame;
	
	float m_totalAnimationTimeSecond;

	float m_animationSpeedModifier;
	bool m_invertedAnimation;
	
	std::vector<Matrix> m_transforms;
};

#endif //ANIMATION_INFO_H
