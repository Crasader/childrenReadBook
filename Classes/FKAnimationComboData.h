#ifndef __ANIMATION_COMBO_DATA_H__
#define __ANIMATION_COMBO_DATA_H__

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationComboData {
public:
    void clear();
    
    void setAnimationType(string sAnimationType);
    string getAnimationType();
    
    void setAnimationIds(string sAnimationIds);
    string getAniamtionIds();
private:
    string                      _sAnimationType;
    string                      _sAnimationIds;
};

NS_FK_END

#endif /* __ANIMATION_COMBO_DATA_H__ */
