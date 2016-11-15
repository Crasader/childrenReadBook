#ifndef __ANIMATION_GROUP_MEMBER_CONTROLLER_H__
#define __ANIMATION_GROUP_MEMBER_CONTROLLER_H__

#include "cocos2d.h"
#include "FKAnimationGroupMemberData.h"
#include "FKAnimationController.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationGroupMemberController
{
public:
    
    AnimationGroupMemberController();
    ~AnimationGroupMemberController();
    
    //获取AnimationGroupMember中的action对象，不区分event的类型，目前适用于auto动画的获取
    Action* getActionFromAnimationGroupMemberData(AnimationGroupMemberData &tmp, string categoryId);
    /**
     *  获取AnimationGroupMember中event为send的所有action对象
     *
     *  @param 单个AnimationGroupMemberData
     *
     *  @return Action*
     */
    
    Action* getSendActionFromAnimationGroupMemberDataKey(AnimationGroupMemberDataKey &keyTmp, string categoryId);
    
    /**
     *  获取AnimationGroupMember中event为Receive的所有action对象
     *
     *  @param 单个AnimationGroupMemberData
     *
     *  @return Action*
     */
    
    Action* getReceiveActionFromAnimationGroupMemberDataValue(AnimationGroupMemberDataValue &valueTmp, string categoryId);
    
    //获取动作序列
    Vector<FiniteTimeAction*> getActionVector(vector<AnimationData> &vAnimationDataTmp, string categoryId);
    
    FiniteTimeAction* getComboActionFromAnimationGroupMemberDataValue(string sAnimationGroupType, Vector<FiniteTimeAction*> &vAction, int time);
    
    vector<string> split( string str, string pattern);
    
private:
};

NS_FK_END

#endif // __ANIMATION_GROUP_MEMBER_CONTROLLER_H__
