#ifndef __ANIMATION_CONTROLLER_H__
#define __ANIMATION_CONTROLLER_H__

#include "cocos2d.h"
#include "FKBookData.h"
#include "FKBookParser.h"
#include "FKDiyRotateBy.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationController
{
public:
    
    AnimationController();
    ~AnimationController();
    
    /**
     通过AnimationData获取到相应的action
     */
    static FiniteTimeAction* getActionFromAnimationData(AnimationData tmpData);
};

NS_FK_END

#endif // __ANIMATION_CONTROLLER_H__
