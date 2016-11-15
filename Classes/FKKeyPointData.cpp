#include "FKKeyPointData.h"

NS_FK_BEGIN

KeyPointData::KeyPointData()
{
    
}
KeyPointData::~KeyPointData()
{
    
}
void KeyPointData::clear()
{
    _fPositionX = 0.0f;
    _fPositionY = 0.0f;
    _iCategory = 0;
    _vAnimationData.clear();
    _sPointType.clear();

}
void KeyPointData::setPosition(string sPositionX, string sPositionY,float coordinateScale,Vec2 winSizeOffset)
{
    _fPositionX = stringTo<float>(sPositionX)*coordinateScale - winSizeOffset.x;
    _fPositionY = stringTo<float>(sPositionY)*coordinateScale - winSizeOffset.y;
}
void KeyPointData::setCategory(string sCategory)
{
    _iCategory = stringTo<int>(sCategory);
    
}
void KeyPointData::setPointType(string sPointType)
{
    _sPointType = sPointType;
}

string KeyPointData::getPointType()
{
    return _sPointType;
}
Vec2 KeyPointData::getPosition()
{
    return Vec2(_fPositionX,_fPositionY);
}
int KeyPointData::getCategory()
{
    return _iCategory;
}
//动画

void KeyPointData::setAnimationData(AnimationData &animationData)
{
    _vAnimationData.push_back(animationData);
}
vector<AnimationData> KeyPointData::getAnimationData()
{
    return _vAnimationData;
}
void KeyPointData::setSpriteTag(string sSpriteTag)
{
    _iSpriteTag = stringTo<int>(sSpriteTag);
}
int KeyPointData::getSpriteTag()
{
    return _iSpriteTag;
}
void KeyPointData::setAnimationType(string sAnimationType)
{
    _sAnimationType = sAnimationType;
}
string KeyPointData::getAnimationType()
{
    return _sAnimationType;
}
void KeyPointData::setTimes(string times)
{
    _sTimes = times;
}
int KeyPointData::getTimes()
{
    return stringTo<int>(_sTimes);;
}

NS_FK_END