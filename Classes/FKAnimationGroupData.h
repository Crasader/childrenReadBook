#ifndef __ANIMATION_GROUP_DATA_H__
#define __ANIMATION_GROUP_DATA_H__

#include "cocos2d.h"
#include "FKAnimationGroupMemberData.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationGroupData
{
public:
    
    AnimationGroupData();
    ~AnimationGroupData();
    
    void clear();
    
    void setAnimationGroupMemberDataMap(AnimationGroupMemberDataKey &key, vector<AnimationGroupMemberDataValue> &value);
    
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> getAnimationGroupMemberData();
private:
    
    //将event为send的AnimationGroupMemberData作为key，将event为receive的AnimationGroupMemberData作为value
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> _mAnimationGroupMemberData;
    
};

NS_FK_END


#endif // __ANIMATION_GROUP_DATA_H__
