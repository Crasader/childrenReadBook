#ifndef __ANIMATION_GROUP_CONTROLLER_H__
#define __ANIMATION_GROUP_CONTROLLER_H__

#include "cocos2d.h"
#include "FKBookData.h"
#include "FKStandardController.h"
#include "FKAnimationGroupMemberData.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationGroupController
{
public:
    
    AnimationGroupController();
    ~AnimationGroupController();
    
    
    
    void setAnimationGroupDataVector(vector<AnimationGroupData> vTmp);
    
    vector<AnimationGroupData> getAnimationGroupDataVector();
    
    /**
     *  通过spriteTag获取对应tag的AnimationGroupMemberData
     *
     *  @param iSpriteTag spriteTag
     *
     *  @return map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>>
     */
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> getAnimationGroupMemberDataFromSpriteTag(int iSpriteTag);
    
    //注：当category为auto时不涉及关联动画，没有event为receive的情况出现。
    //get所有animationCategoryId为auto的AnimationGroupMemberKeyData
    vector<AnimationGroupMemberData> getAutoAnimationGroupMemberKeyData();
private:
    vector<AnimationGroupData> _vAnimationGroupData;
};


NS_FK_END

#endif // __ANIMATION_GROUP_CONTROLLER_H__
