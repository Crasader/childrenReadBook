#ifndef __ANIMATION_GROUP_SET_DATA_H__
#define __ANIMATION_GROUP_SET_DATA_H__

#include "cocos2d.h"
#include "FKAnimationGroupData.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationGroupSetData
{
public:
    
    AnimationGroupSetData();
    ~AnimationGroupSetData();
    
    void setAnimationGroupData(AnimationGroupData &animationGroupData);
    vector<AnimationGroupData>  getAnimationGroupData();
    
    void clear();
private:
    vector<AnimationGroupData> _vAnimationGroupData;
    
};

NS_FK_END


#endif // __ANIMATION_GROUP_SET_DATA_H__
