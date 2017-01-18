#ifndef ANIMATION_H
#define	ANIMATION_H

#include <string>
#include <vector>
#include <hash_map>
#include <hash_set>

#include "../../Math/Matrix.h"


class Bone 
{
public:

	std::string m_boneName;
	Matrix m_offset, m_localTransform, m_globalTransform, m_originalLocalTransform, m_finalTransform;
	Bone* m_parent;
	std::vector<Bone*> mp_children;

	Bone() :m_parent(0){}
	~Bone()
	{ 
		for(size_t i(0), childenSize(mp_children.size()); i < childenSize; ++i) 
			delete mp_children[i];
	}
};

class AnimationChannel
{
public:
	std::string m_name;
	std::vector<aiVectorKey> m_positionKeys;
	std::vector<aiQuatKey> m_rotationKeys;
	std::vector<aiVectorKey> m_scalingKeys;
};

class Animation
{
public:

	Animation(): m_ticksPerSecond(0.0f), m_duration(0.0f), m_playAnimationForward(true) {}
	Animation( const aiAnimation* ip_anim);
	std::vector<Matrix>& getTransforms(float i_dt){ return m_transforms[getFrameIndexAt(i_dt)]; }
	unsigned int getFrameIndexAt(float i_time);

	std::string m_name;
	std::vector<AnimationChannel> m_channels;

	// play forward == true, play backward == false
	bool m_playAnimationForward;
	float m_ticksPerSecond, m_duration;	

	
	std::vector<std::vector<Matrix>> m_transforms;

	std::hash_map<std::string, int> m_nameToIndexChannel;

	std::vector<std::vector<Matrix>> m_bakedTranslation;
	std::vector<std::vector<aiQuaternion>> m_bakedRotation;
	std::vector<std::vector<Matrix>> m_bakedScaling;
};

#endif //ANIMATION_H