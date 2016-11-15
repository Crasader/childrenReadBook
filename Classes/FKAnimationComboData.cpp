#include "FKAnimationComboData.h"

NS_FK_BEGIN

void AnimationComboData::clear()
{
    _sAnimationIds.clear();
    _sAnimationType.clear();
}

void AnimationComboData::setAnimationType(string sAnimationType)
{
    _sAnimationType = sAnimationType;
}

string AnimationComboData::getAnimationType()
{
    return _sAnimationType;
}

void AnimationComboData::setAnimationIds(string sAnimationIds)
{
    _sAnimationIds = sAnimationIds;
}

string AnimationComboData::getAniamtionIds()
{
    return _sAnimationIds;
}

NS_FK_END
