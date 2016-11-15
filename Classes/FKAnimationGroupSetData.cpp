#include "FKAnimationGroupSetData.h"

NS_FK_BEGIN

AnimationGroupSetData::AnimationGroupSetData()
{
    
}
AnimationGroupSetData::~AnimationGroupSetData()
{
    
}
void AnimationGroupSetData::clear()
{
    _vAnimationGroupData.clear();
}

void AnimationGroupSetData::setAnimationGroupData(AnimationGroupData &animationGroupData)
{
    _vAnimationGroupData.push_back(animationGroupData);
}

vector<AnimationGroupData> AnimationGroupSetData::getAnimationGroupData()
{
    return _vAnimationGroupData;
}

NS_FK_END



