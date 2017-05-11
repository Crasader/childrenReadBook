#include "FKSpriteData.h"

NS_FK_BEGIN

SpriteData::SpriteData()
{
    
}
SpriteData::~SpriteData()
{
    
}

void SpriteData::clear()
{
    _sImageId.clear();
    _sSoundId.clear();
    _sTouchEnable.clear();
    _fCenterPointX = 0.0f;
    _fCenterPointY = 0.0f;
    _fAnchorPointX = 0.0f;
    _fAnchorPointY = 0.0f;
    _fEndPositionX = 0.0f;
    _fEndPositionY = 0.0f;
    _iTag = 0;
    _iZOrder = 0;
    _fOpacity = 0.0f;
    _fRotation = 0.0f;
    _fScale = 0.0f;
    _sIsAlpha.clear();
}

/**************SET***********/
void SpriteData::setImageId(string sImageId)
{
    _sImageId = sImageId;
}

void SpriteData::setSoundId(string sSoundId)
{
    _sSoundId = sSoundId;
}

void SpriteData::setTouchEnable(string sTouchEnable)
{
    _sTouchEnable = sTouchEnable;
}

void SpriteData::setCenterPoint(string sCenterPointX, string sCenterPointY,float coordinateScale,Vec2 winSizeOffset)
{
    _fCenterPointX = stringTo<float>(sCenterPointX)*coordinateScale - winSizeOffset.x;
    _fCenterPointY = stringTo<float>(sCenterPointY)*coordinateScale - winSizeOffset.y;
}

void SpriteData::setAnchorPoint(string sAnchorPointX, string sAnchorPointY)
{
    _fAnchorPointX = stringTo<float>(sAnchorPointX);
    _fAnchorPointY = stringTo<float>(sAnchorPointY);
}

void SpriteData::setEndPosition(string sEndPositionX, string sEndPositionY,float coordinateScale,Vec2 winSizeOffset)
{
    _fEndPositionX = stringTo<float>(sEndPositionX)*coordinateScale - winSizeOffset.x;
    _fEndPositionY = stringTo<float>(sEndPositionY)*coordinateScale - winSizeOffset.y;
}

void SpriteData::setTag(string sTag)
{
    _iTag = stringTo<int>(sTag);
}

void SpriteData::setZOrder(string sZOrder)
{
    _iZOrder = stringTo<int>(sZOrder);
}

void SpriteData::setOpacity(string sOpacity)
{
    _fOpacity = stringTo<float>(sOpacity);
}

void SpriteData::setRotation(string sRotation)
{
    _fRotation = stringTo<float>(sRotation);
}

void SpriteData::setScale(string sScale, float resourcesScale)
{
    _fScale = stringTo<float>(sScale)*resourcesScale;
}

void SpriteData::setIsAlpha(string sIsAlpha)
{
    _sIsAlpha = sIsAlpha;
}

/************GET***********/
string SpriteData::getImageId()
{
    return _sImageId;
}

string SpriteData::getSoundId()
{
    return _sSoundId;
}
Vec2 SpriteData::getPosition()
{
    return Vec2(_fCenterPointX, _fCenterPointY);
}
Vec2 SpriteData::getAnchorPoint()
{
    return Vec2(_fAnchorPointX, _fAnchorPointY);
}
Vec2 SpriteData::getEndPosition()
{
    return Vec2(_fEndPositionX, _fEndPositionY);
}
int SpriteData::getTag()
{
    return _iTag;
}
int SpriteData::getZOrder()
{
    return _iZOrder;
}
float SpriteData::getOpacity()
{
    return _fOpacity;
}
float SpriteData::getRotation()
{
    return _fRotation;
}
float SpriteData::getScale()
{
    return _fScale;
}
string SpriteData::getTouchEnable()
{
    return _sTouchEnable;
}
string SpriteData::getIsAlpha()
{
    return _sIsAlpha;
}

NS_FK_END





