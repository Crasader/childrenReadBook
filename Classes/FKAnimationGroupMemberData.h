#ifndef __ANIMATION_GROUP_MEMBER_DATA_H__
#define __ANIMATION_GROUP_MEMBER_DATA_H__

#include "cocos2d.h"
#include "FKAnimationData.h"
#include "FKAnimationComboData.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationGroupMemberData
{
public:
    AnimationGroupMemberData();
    ~AnimationGroupMemberData();
    
    void                        setAnimationComboData(AnimationComboData &animationComboData);
    void                        setEvent(string sEvent);
    void                        setSpriteTag(string sSpriteTag);
    void                        setAnimationType(string sAnimationGroupType);
    void                        setAnimationData(AnimationData &animationData);
    void                        setTimes(string times);
    
    string                      getEvent();
    int                         getSpriteTag();
    string                      getAnimationGroupType();
    vector<AnimationData>       getAnimationData();
    int                         getTimes();
    vector<AnimationComboData>  getAnimationComboData();
    
    void                        clear();
    
private:
    string                      _sTimes;
    string                      _sEvent;
    string                      _sSpriteTag;
    string                      _sAnimationGroupType;
    vector<AnimationData>       _vAnimationData;
    vector<AnimationComboData>  _vAnimationComboData;
};

class FK_DLL AnimationGroupMemberDataKey : public AnimationGroupMemberData
{
public:
    
    AnimationGroupMemberDataKey();
    ~AnimationGroupMemberDataKey();
    
    //重载
    bool operator < (const AnimationGroupMemberDataKey &c) const { return true; };
};

class FK_DLL AnimationGroupMemberDataValue : public AnimationGroupMemberData
{
public:
    AnimationGroupMemberDataValue();
    ~AnimationGroupMemberDataValue();
    
};

NS_FK_END

#endif // __ANIMATION_GROUP_MEMBER_DATA_H__
