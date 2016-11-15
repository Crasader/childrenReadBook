#include "FKAnimationGroupMemberData.h"

NS_FK_BEGIN

#pragma mark - AnimationGroupMemberData
AnimationGroupMemberData::AnimationGroupMemberData()
{
    
}
AnimationGroupMemberData::~AnimationGroupMemberData()
{
    
}
void AnimationGroupMemberData::clear()
{
    _sTimes.clear();
    _sEvent.clear();
    _sSpriteTag.clear();
    _sAnimationGroupType.clear();
    _vAnimationData.clear();
    _vAnimationComboData.clear();
}
string AnimationGroupMemberData::getEvent()
{
    return _sEvent;
}
int AnimationGroupMemberData::getSpriteTag()
{
    return stringTo<int>(_sSpriteTag);
}
vector<AnimationComboData> AnimationGroupMemberData::getAnimationComboData()
{
    return _vAnimationComboData;
}
string AnimationGroupMemberData::getAnimationGroupType()
{
    return _sAnimationGroupType;
}

vector<AnimationData> AnimationGroupMemberData::getAnimationData()
{
    return _vAnimationData;
}

int AnimationGroupMemberData::getTimes()
{
    return stringTo<int>(_sTimes);
}

void AnimationGroupMemberData::setAnimationComboData(AnimationComboData &animationComboData)
{
    _vAnimationComboData.push_back(animationComboData);
}
void AnimationGroupMemberData::setEvent(string sEvent)
{
    _sEvent = sEvent;
}

void AnimationGroupMemberData::setSpriteTag(string sSpriteTag)
{
    _sSpriteTag = sSpriteTag;
}

void AnimationGroupMemberData::setAnimationType(string sAnimationGroupType)
{
    _sAnimationGroupType = sAnimationGroupType;
}

void AnimationGroupMemberData::setAnimationData(AnimationData &animationData)
{
    _vAnimationData.push_back(animationData);
}

void AnimationGroupMemberData::setTimes(string times)
{
    _sTimes = times;
}

#pragma mark - AnimationGroupMemberDataKey
AnimationGroupMemberDataKey::AnimationGroupMemberDataKey()
{
    
}
AnimationGroupMemberDataKey::~AnimationGroupMemberDataKey()
{
    
}


#pragma mark - AnimationGroupMemberDataValue
AnimationGroupMemberDataValue::AnimationGroupMemberDataValue()
{
    
}
AnimationGroupMemberDataValue::~AnimationGroupMemberDataValue()
{
    
}

NS_FK_END






