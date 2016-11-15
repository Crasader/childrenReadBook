#include "FKNormalPointData.h"

NS_FK_BEGIN

NormalPointData::NormalPointData()
{
    
}
NormalPointData::~NormalPointData()
{
    
}
void NormalPointData::clear()
{
    _iNumber = 0;
    _iSpriteTag = -2;
    _stPosition.x = 0;
    _stPosition.y = 0;
    _sAnimationType.clear();
    _vAnimationData.clear();
}
void NormalPointData::setNumber(string sNumber)
{
    _iNumber = stringTo<int>(sNumber);
}
void NormalPointData::setPosition(string sPositionX, string sPositionY,float coordinateScale,Vec2 winSizeOffset)
{
    _stPosition.x = stringTo<float>(sPositionX)*coordinateScale - winSizeOffset.x;
    _stPosition.y = stringTo<float>(sPositionY)*coordinateScale - winSizeOffset.y;
}
int NormalPointData::getNumber()
{
    return _iNumber;
}
Vec2 NormalPointData::getPosition()
{
    return Vec2(_stPosition);
}
//动画部分
void NormalPointData::setAnimationData(AnimationData &animationData)
{
    _vAnimationData.push_back(animationData);
}
vector<AnimationData> NormalPointData::getAnimationData()
{
    return _vAnimationData;
}
void NormalPointData::setSpriteTag(string sSpriteTag)
{
    _iSpriteTag = stringTo<int>(sSpriteTag);
}
int NormalPointData::getSpriteTag()
{
    return _iSpriteTag;
}
void NormalPointData::setAnimationType(string sAnimationType)
{
    _sAnimationType = sAnimationType;
}
string NormalPointData::getAnimationType()
{
    return _sAnimationType;
}
void NormalPointData::setTimes(string times)
{
    _sTimes = times;
}
int NormalPointData::getTimes()
{
    return stringTo<int>(_sTimes);;
}
NS_FK_END

