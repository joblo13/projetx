#include "Animation.h"

Animation::Animation( const aiAnimation* ip_anim) 
{
	m_ticksPerSecond = static_cast<float>(ip_anim->mTicksPerSecond != 0.0f ? ip_anim->mTicksPerSecond : 100.0f);
	m_duration = static_cast<float>(ip_anim->mDuration);
	m_name = ip_anim->mName.data;

	m_channels.resize(ip_anim->mNumChannels);
	for( unsigned int i(0), numChannels(ip_anim->mNumChannels); i < numChannels; ++i)
	{		
		m_channels[i].m_name = ip_anim->mChannels[i]->mNodeName.data;
		m_nameToIndexChannel.insert(std::pair<std::string, int>(m_channels[i].m_name, i));

		for(unsigned int j(0); j < ip_anim->mChannels[i]->mNumPositionKeys; j++) 
			m_channels[i].m_positionKeys.push_back(ip_anim->mChannels[i]->mPositionKeys[j]);

		for(unsigned int j(0); j < ip_anim->mChannels[i]->mNumRotationKeys; j++)
			m_channels[i].m_rotationKeys.push_back(ip_anim->mChannels[i]->mRotationKeys[j]);

		for(unsigned int j(0); j < ip_anim->mChannels[i]->mNumScalingKeys; j++) 
			m_channels[i].m_scalingKeys.push_back(ip_anim->mChannels[i]->mScalingKeys[j]);
	}
}

unsigned int Animation::getFrameIndexAt(float i_time)
{
	// get a [0.f ... 1.f) value by allowing the percent to wrap around 1
	i_time *= m_ticksPerSecond;

	float time = 0.0f;
	if( m_duration > 0.0f)
		time = fmod( i_time, m_duration);

	float percent = time / m_duration;
	// this will invert the percent so the animation plays backwards
	if(!m_playAnimationForward) 
		percent = (percent-1.0f)*-1.0f;

	return static_cast<unsigned int>(( static_cast<float>(m_transforms.size()) * percent));
}