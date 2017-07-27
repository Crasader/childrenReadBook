#include "FKAnimationData.h"

NS_FK_BEGIN

AnimationData::AnimationData()
{
    
}
AnimationData::~AnimationData()
{
    
}

void AnimationData::clear()
{
    _sAnimationId.clear();
    _sStyleId.clear();
    _sCategoryId.clear();
    _sDuration.clear();
    _stEndPosition = Vec2();
    _fAngle = 0.0f;
    _fScale = 0.0f;
    _fHeight = 0.0f;
    _iJumps = 0;
    _iBlinks = 0;
    _fRed = 0.0f;
    _fGreen = 0.0f;
    _fBlue = 0.0f;
    _fOpacity = 0.0f;
    _stControlPoint_1 = Vec2();
    _stControlPoint_2 = Vec2();
    _sImageId.clear();
    _sSoundId.clear();
    _iFramecount = 0;
    _stPosition = Vec2();
    _fScaleX = 0.0f;
    _fScaleY = 0.0f;
    _fAngleX = 0.0f;
    _fAngleY = 0.0f;
    _bTurn = false;
    _stCenter = Vec2();
    _fDegrees = 0.0f;
    _fRadius = 0.0f;
    _fRate = 0.0f;
    _sStyle.clear();
    _fDelay = 0.0f;
    _iCount = 0;
    _iConstentX = 0;
    _iConstentY = 0;
}

#pragma mark - get
string AnimationData::getAnimationId()
{
    return _sAnimationId;
}
string AnimationData::getStyleId()
{
    return _sStyleId;
}
string AnimationData::getCategoryId()
{
    return _sCategoryId;
}
float AnimationData::getDuration()
{
    return stringTo<float>(_sDuration);
}
Vec2 AnimationData::getEndPosition()
{
    return _stEndPosition;
}
float AnimationData::getAngle()
{
    return _fAngle;
}
float AnimationData::getScale()
{
    return _fScale;
}
float AnimationData::getHeight()
{
    return _fHeight;
}
int AnimationData::getJumps()
{
    return _iJumps;
}
int AnimationData::getBlinks()
{
    return _iBlinks;
}
Color3B AnimationData::getRGBColor()
{
    Color3B  _stColor3B;
    _stColor3B.r = _fRed;
    _stColor3B.g = _fGreen;
    _stColor3B.b = _fBlue;
    return _stColor3B;
}
float AnimationData::getOpacity()
{
    return _fOpacity;
}
Vec2 AnimationData::getBezierPointOne()
{
    return _stControlPoint_1;
}
Vec2 AnimationData::getBezierPointTwo()
{
    return _stControlPoint_2;
}
string AnimationData::getImageId()
{
    return _sImageId;
}
string AnimationData::getSoundId()
{
    return _sSoundId;
}
int AnimationData::getFramecount()
{
    return _iFramecount;
}
Vec2 AnimationData::getPosition()
{
    return _stPosition;
}
float AnimationData::getScaleX()
{
    return _fScaleX;
}
float AnimationData::getScaleY()
{
    return _fScaleY;
}
float AnimationData::getAngleX()
{
    return _fAngleX;
}
float AnimationData::getAngleY()
{
    return _fAngleY;
}
bool AnimationData::getTurn()
{
    return _bTurn;
}
Vec2 AnimationData::getCenter()
{
    return _stCenter;
}
float AnimationData::getDegrees()
{
    return _fDegrees;
}
float AnimationData::getRadius()
{
    return _fRadius;
}
float AnimationData::getRate()
{
    return _fRate;
}
string AnimationData::getStyle()
{
    return _sStyle;
}
float AnimationData::getDelay()
{
    return _fDelay;
}
int AnimationData::getCount()
{
    return _iCount;
}
int AnimationData::getConstentX()
{
    return _iConstentX;
}
int AnimationData::getConstentY()
{
    return _iConstentY;
}




#pragma mark - set

void AnimationData::setAnimationId(string sAnimationId)
{
    _sAnimationId = sAnimationId;
}
void AnimationData::setStyleId(string sStyleId)
{
    _sStyleId = sStyleId;
}
void AnimationData::setCategoryId(string sCategoryId)
{
    _sCategoryId = sCategoryId;
}
void AnimationData::setDuration(string sDuration)
{
    _sDuration = sDuration;
}
void AnimationData::setEndPosition(string sX, string sY, float coordinateScale,Vec2 winSizeOffset, bool isRelative)
{
    if (isRelative) {
        _stEndPosition.x = stringTo<float>(sX)*coordinateScale;
        _stEndPosition.y = stringTo<float>(sY)*coordinateScale;
    }else
    {
        _stEndPosition.x = stringTo<float>(sX)*coordinateScale - winSizeOffset.x;
        _stEndPosition.y = stringTo<float>(sY)*coordinateScale - winSizeOffset.y;
    }
}
void AnimationData::setAngle(string sAngle)
{
    _fAngle = stringTo<float>(sAngle);
}
void AnimationData::setScale(string sScale,float resourcesScale)
{
    _fScale = stringTo<float>(sScale)*resourcesScale;
}
void AnimationData::setHeight(string sHeight,float coordinateScale)
{
    _fHeight = stringTo<float>(sHeight)*coordinateScale;
}
void AnimationData::setJumps(string sJumps)
{
    _iJumps = stringTo<int>(sJumps);
}
void AnimationData::setBlinks(string sBlinks)
{
    _iBlinks = stringTo<int>(sBlinks);
}
void AnimationData::setRGBColor(string sR, string sG, string sB)
{
    _fRed = stringTo<float>(sR);
    _fGreen = stringTo<float>(sG);
    _fBlue = stringTo<float>(sB);
}
void AnimationData::setOpacity(string sOpacity)
{
    _fOpacity = stringTo<float>(sOpacity);
}
void AnimationData::setBezierPoint(string sPointOneX, string sPointOneY, string sPointTwoX, string sPointTwoY, float coordinateScale,Vec2 winSizeOffset)
{
    _stControlPoint_1.x = stringTo<float>(sPointOneX)*coordinateScale - winSizeOffset.x;
    _stControlPoint_1.y = stringTo<float>(sPointOneY)*coordinateScale - winSizeOffset.y;
    _stControlPoint_2.x = stringTo<float>(sPointTwoX)*coordinateScale - winSizeOffset.x;
    _stControlPoint_2.y = stringTo<float>(sPointTwoY)*coordinateScale - winSizeOffset.y;
}
void AnimationData::setImageId(string sImageId)
{
    _sImageId = sImageId;
}
void AnimationData::setSoundId(string sSoundId)
{
    _sSoundId = sSoundId;
}
void AnimationData::setFramecount(string sFramecount)
{
    _iFramecount = stringTo<int>(sFramecount);
}
void AnimationData::setPosition(string sPositionX,string sPositionY,float coordinateScale,Vec2 winSizeOffset)
{
    _stPosition.x = stringTo<float>(sPositionX)*coordinateScale - winSizeOffset.x;
    _stPosition.y = stringTo<float>(sPositionY)*coordinateScale - winSizeOffset.y;
}
void AnimationData::setScaleXY(string sScaleX,string sScaleY,float resourcesScale)
{
    _fScaleX = stringTo<float>(sScaleX)*resourcesScale;
    _fScaleY = stringTo<float>(sScaleY)*resourcesScale;
    
}
void AnimationData::setRotateXY(string sAngleX,string sAngleY)
{
    _fAngleX = stringTo<float>(sAngleX);
    _fAngleY = stringTo<float>(sAngleY);
    
}
void  AnimationData::setTurn(string sTurn)
{
    _bTurn = stringTo<bool>(sTurn);
}
void AnimationData::setCenter(string sCenterX,string sCenterY,float coordinateScale,Vec2 winSizeOffset)
{
    _stCenter.x = stringTo<float>(sCenterX)*coordinateScale - winSizeOffset.x;
    _stCenter.y = stringTo<float>(sCenterY)*coordinateScale - winSizeOffset.y;
}
void AnimationData::setDegrees(string sDegrees)
{
    _fDegrees = stringTo<float>(sDegrees);
}
void AnimationData::setRadius(string sRadius)
{
    _fRadius = stringTo<float>(sRadius);
}
void AnimationData::setRate(string sRate)
{
    _fRate = stringTo<float>(sRate);
}
void  AnimationData::setStyle(string sStyle)
{
    _sStyle = stringTo<string>(sStyle);
}
void  AnimationData::setDelay(string sDelay)
{
    _fDelay = stringTo<float>(sDelay);
}
void  AnimationData::setCount(string sCount)
{
    _iCount = stringTo<int>(sCount);
    
}
void  AnimationData::setConstent(string sConstentX,string sConstentY,float coordinateScale)
{
    _iConstentX = stringTo<int>(sConstentX)*coordinateScale;
    _iConstentY = stringTo<int>(sConstentY)*coordinateScale;
    
}

NS_FK_END








