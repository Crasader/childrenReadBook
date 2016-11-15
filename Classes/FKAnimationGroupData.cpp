#include "FKAnimationGroupData.h"

NS_FK_BEGIN

AnimationGroupData::AnimationGroupData()
{
    
}
AnimationGroupData::~AnimationGroupData()
{
    
}




void AnimationGroupData::clear()
{
    _mAnimationGroupMemberData.clear();
}

void AnimationGroupData::setAnimationGroupMemberDataMap(AnimationGroupMemberDataKey &key, vector<AnimationGroupMemberDataValue> &value)
{
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> mTmp;
    mTmp[key] = value;
    _mAnimationGroupMemberData = mTmp;
}

map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> AnimationGroupData::getAnimationGroupMemberData()
{
    return _mAnimationGroupMemberData;
}

NS_FK_END



