#include "FKAnimationGroupMemberController.h"

NS_FK_BEGIN

AnimationGroupMemberController::AnimationGroupMemberController()
{
    
}
AnimationGroupMemberController::~AnimationGroupMemberController()
{
    
}

Action* AnimationGroupMemberController::getActionFromAnimationGroupMemberData(AnimationGroupMemberData &tmp, string categoryId)
{
    Vector<FiniteTimeAction*> vAction;
    string sAnimationGroupType = tmp.getAnimationGroupType();
    vector<AnimationData> vAnimationDataTmp = tmp.getAnimationData();
    
    vector<AnimationComboData> vAnimatioComboData = tmp.getAnimationComboData();
    if (vAnimatioComboData.size() > 0) {
        for (unsigned int i = 0; i < vAnimatioComboData.size(); i++) {
            AnimationComboData animationComboData = vAnimatioComboData[i];
            string animationIds = animationComboData.getAniamtionIds();
            string sAnimationType = animationComboData.getAnimationType();
            vector<string> vAnimationId = split(animationIds, "-");
            if (vAnimationId.size() > 0) {
                Vector<FiniteTimeAction*> vComboAction;
                for (unsigned int j = 0; j < vAnimationId.size(); j++) {
                    string animationId = vAnimationId[j];
                    for (unsigned int k = 0; k < vAnimationDataTmp.size(); k++) {
                        AnimationData animationData = vAnimationDataTmp[k];
                        if (animationData.getAnimationId() == animationId) {
                            vComboAction.pushBack(AnimationController::getActionFromAnimationData(animationData));
                        }
                    }
                }
                vAction.pushBack(getComboActionFromAnimationGroupMemberDataValue(sAnimationType, vComboAction, tmp.getTimes()));
            }
        }
        
    }else
    {
        vAction = getActionVector(vAnimationDataTmp, categoryId);
    }
    
    return getComboActionFromAnimationGroupMemberDataValue(sAnimationGroupType, vAction, tmp.getTimes());
}

Action* AnimationGroupMemberController::getSendActionFromAnimationGroupMemberDataKey(AnimationGroupMemberDataKey &keyTmp, string categoryId)
{
    Vector<FiniteTimeAction*> vAction;
    string sAnimationGroupType = keyTmp.getAnimationGroupType();
    vector<AnimationData> vAnimationDataTmp = keyTmp.getAnimationData();
    
    vector<AnimationComboData> vAnimatioComboData = keyTmp.getAnimationComboData();
    if (vAnimatioComboData.size() > 0) {
        for (unsigned int i = 0; i < vAnimatioComboData.size(); i++) {
            AnimationComboData animationComboData = vAnimatioComboData[i];
            string animationIds = animationComboData.getAniamtionIds();
            string sAnimationType = animationComboData.getAnimationType();
            vector<string> vAnimationId = split(animationIds, "-");
            if (vAnimationId.size() > 0) {
                Vector<FiniteTimeAction*> vComboAction;
                for (unsigned int j = 0; j < vAnimationId.size(); j++) {
                    string animationId = vAnimationId[j];
                    for (unsigned int k = 0; k < vAnimationDataTmp.size(); k++) {
                        AnimationData animationData = vAnimationDataTmp[k];
                        if (animationData.getAnimationId() == animationId) {
                            vComboAction.pushBack(AnimationController::getActionFromAnimationData(animationData));
                        }
                    }
                }
                vAction.pushBack(getComboActionFromAnimationGroupMemberDataValue(sAnimationType, vComboAction, keyTmp.getTimes()));
            }
        }
        
    }else
    {
        vAction = getActionVector(vAnimationDataTmp, categoryId);
    }
    
    return getComboActionFromAnimationGroupMemberDataValue(sAnimationGroupType, vAction, keyTmp.getTimes());
    
}

Action* AnimationGroupMemberController::getReceiveActionFromAnimationGroupMemberDataValue(AnimationGroupMemberDataValue &valueTmp, string categoryId)
{
    Vector<FiniteTimeAction*> vAction;
    string sAnimationGroupType = valueTmp.getAnimationGroupType();
    vector<AnimationData> vAnimationDataTmp = valueTmp.getAnimationData();
    
    vector<AnimationComboData> vAnimatioComboData = valueTmp.getAnimationComboData();
    if (vAnimatioComboData.size() > 0) {
        for (unsigned int i = 0; i < vAnimatioComboData.size(); i++) {
            AnimationComboData animationComboData = vAnimatioComboData[i];
            string animationIds = animationComboData.getAniamtionIds();
            string sAnimationType = animationComboData.getAnimationType();
            vector<string> vAnimationId = split(animationIds, "-");
            if (vAnimationId.size() > 0) {
                Vector<FiniteTimeAction*> vComboAction;
                for (unsigned int j = 0; j < vAnimationId.size(); j++) {
                    string animationId = vAnimationId[j];
                    for (unsigned int k = 0; k < vAnimationDataTmp.size(); k++) {
                        AnimationData animationData = vAnimationDataTmp[k];
                        if (animationData.getAnimationId() == animationId) {
                            vComboAction.pushBack(AnimationController::getActionFromAnimationData(animationData));
                        }
                    }
                }
                vAction.pushBack(getComboActionFromAnimationGroupMemberDataValue(sAnimationGroupType, vComboAction, valueTmp.getTimes()));
            }
        }
        
    }else
    {
        vAction = getActionVector(vAnimationDataTmp, categoryId);
    }
    
    return getComboActionFromAnimationGroupMemberDataValue(sAnimationGroupType, vAction, valueTmp.getTimes());
}

Vector<FiniteTimeAction*> AnimationGroupMemberController::getActionVector(vector<AnimationData> &vAnimationDataTmp, string categoryId)
{
    Vector<FiniteTimeAction*> vAction;
    
    for (size_t i = 0; i < vAnimationDataTmp.size(); i++) {
        AnimationData animationData = vAnimationDataTmp[i];
        if (animationData.getCategoryId() == categoryId) {
            vAction.pushBack(AnimationController::getActionFromAnimationData(animationData));
        }
    }
    return vAction;
}


FiniteTimeAction* AnimationGroupMemberController::getComboActionFromAnimationGroupMemberDataValue(string sAnimationGroupType, Vector<FiniteTimeAction*> &vAction, int time)
{
    FiniteTimeAction* ac = nullptr;
    if (sAnimationGroupType == "spawn" && vAction.size() > 0) {
        ac = Spawn::create(vAction);
    }
    else if (sAnimationGroupType == "sequence" && vAction.size() > 0)
    {
        ac = Sequence::create(vAction);
    }
    else if (sAnimationGroupType == "repeat" && vAction.size() > 0)
    {
        ac = Repeat::create(Sequence::create(vAction), time);
    }
    else if (sAnimationGroupType == "repeatforever" && vAction.size() > 0)
    {
        ac = RepeatForever::create(Sequence::create(vAction));
    }
    else if (sAnimationGroupType == "null" && vAction.size() > 0)
    {
        ac = vAction.at(0);
    }
    return ac;
}


//字符串分割函数
vector<string> AnimationGroupMemberController::split( string str, string pattern)
{
    vector<string> ret;
    if(pattern.empty()) return ret;
    size_t start = 0,index = str.find_first_of(pattern,0);
    while(index!=str.npos)
    {
        if(start!=index)
            ret.push_back(str.substr(start,index-start));
        start = index + 1;
        index = str.find_first_of(pattern,start);
    }
    if(!str.substr(start).empty())
        ret.push_back(str.substr(start));
    return ret;
}

NS_FK_END


