#include "FKAnimationGroupController.h"

NS_FK_BEGIN

AnimationGroupController::AnimationGroupController()
{
    
}
AnimationGroupController::~AnimationGroupController()
{
    
}



void AnimationGroupController::setAnimationGroupDataVector(vector<AnimationGroupData> vTmp)
{
    _vAnimationGroupData = vTmp;
}

vector<AnimationGroupData> AnimationGroupController::getAnimationGroupDataVector()
{
    return _vAnimationGroupData;
}

map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> AnimationGroupController::getAnimationGroupMemberDataFromSpriteTag(int iSpriteTag)
{
    AnimationGroupMemberDataKey keyTmp;
    vector<AnimationGroupMemberDataValue> vValueTmp;
    vector<AnimationData> vAnimationDataTmp;
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> mDataTmp;
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> mRet;
    for (size_t i = 0; i < _vAnimationGroupData.size(); i++) {
        mDataTmp = _vAnimationGroupData[i].getAnimationGroupMemberData();
        for (map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>>::iterator it = mDataTmp.begin(); it != mDataTmp.end(); ++it) {
            keyTmp = it->first;
            vAnimationDataTmp = keyTmp.getAnimationData();
            vValueTmp = it->second;
            if (iSpriteTag == keyTmp.getSpriteTag() && !vAnimationDataTmp.empty()) {
                mRet[it->first] = it->second;
                break;
            }
        }
    }
    return mRet;
    
}

vector<AnimationGroupMemberData> AnimationGroupController::getAutoAnimationGroupMemberKeyData()
{
    AnimationGroupMemberDataKey keyTmp;
    vector<AnimationGroupMemberDataValue> vValueTmp;
    AnimationGroupMemberDataValue valueTmp;
    vector<AnimationGroupMemberData> vAutoTmp;
    vector<AnimationData> vKeyAnimationDataTmp;
    vector<AnimationData> vValueAnimationDataTmp;
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> mDataTmp;
    for (size_t i = 0; i < _vAnimationGroupData.size(); i++) {
        mDataTmp = _vAnimationGroupData[i].getAnimationGroupMemberData();
        for (map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>>::iterator it = mDataTmp.begin(); it != mDataTmp.end(); ++it) {
            keyTmp = it->first;
            vValueTmp = it->second;
            vKeyAnimationDataTmp = keyTmp.getAnimationData();
            if (!vKeyAnimationDataTmp.empty())
            {
                for (unsigned int j = 0; j < vKeyAnimationDataTmp.size(); j++) {
                    if (vKeyAnimationDataTmp[j].getCategoryId() == "auto") {
                        vAutoTmp.push_back(keyTmp);
                        break;
                    }
                }
                
            }
            for (unsigned int k = 0; k < vValueTmp.size(); k++) {
                valueTmp = vValueTmp[k];
                vValueAnimationDataTmp = valueTmp.getAnimationData();
                if (!vValueAnimationDataTmp.empty())
                {
                    for (unsigned int j = 0; j < vValueAnimationDataTmp.size(); j++) {
                        if (vValueAnimationDataTmp[j].getCategoryId() == "auto") {
                            vAutoTmp.push_back(valueTmp);
                            break;
                        }
                    }
                }
            }
        }
    }
    return vAutoTmp;
}

NS_FK_END
