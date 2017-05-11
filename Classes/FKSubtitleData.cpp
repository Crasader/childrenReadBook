#include "FKSubtitleData.h"

NS_FK_BEGIN

SubtitleData::SubtitleData()
{
    
}

SubtitleData::~SubtitleData()
{
    
}

void SubtitleData::clear()
{
    _sImageId.clear();
    _fPositionX = 0.0f;
    _fPositionY = 0.0f;
    _fStartTime = 0.0f;
    _fDuration = 0.0f;
    _iZOrder = 0;
}

void SubtitleData::setImageId(string sImageId)
{
    _sImageId = sImageId;
}

string SubtitleData::getImageId()
{
    return _sImageId;
}

void SubtitleData::setPosition(string sPositionX, string sPositionY, float coordinateScale, cocos2d::Vec2 winSizeOffset)
{
    _fPositionX = stringTo<float>(sPositionX)*coordinateScale - winSizeOffset.x;
    _fPositionY = stringTo<float>(sPositionY)*coordinateScale - winSizeOffset.y;
}

Vec2 SubtitleData::getPosition()
{
    return Vec2(_fPositionX, _fPositionY);
}

void SubtitleData::setZOrder(string sZOrder)
{
    _iZOrder = stringTo<int>(sZOrder);
}

int SubtitleData::getZOrder()
{
    return _iZOrder;
}

void SubtitleData::setStartTime(string sStartTime)
{
    _fStartTime = stringTo<float>(sStartTime);
}

float SubtitleData::getStartTime()
{
    return _fStartTime;
}

void SubtitleData::setDuration(string sDuration)
{
    _fDuration = stringTo<float>(sDuration);
}

float SubtitleData::getDuration()
{
    return _fDuration;
}

void SubtitleData::setAnimationEffect(string sAnimationEffect)
{
    _sAnimationEffect = sAnimationEffect;
}

string SubtitleData::getAnimationEffect()
{
    return _sAnimationEffect;
}
NS_FK_END
