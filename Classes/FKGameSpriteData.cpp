#include "FKGameSpriteData.h"

NS_FK_BEGIN

GameSpriteData::GameSpriteData()
{
    
}
GameSpriteData::~GameSpriteData()
{
    
}

void GameSpriteData::clear()
{
    _sImageId.clear();
    _sSoundId.clear();
    _sTouchEnable.clear();
    _sIsWin.clear();
    _fPositionX = 0.0f;
    _fPositionY = 0.0f;
    _fAnchorPointX = 0.0f;
    _fAnchorPointY = 0.0f;
    _iTag = 0;
    _iZOrder = 0;
    _fOpacity = 0.0f;
    _fRotation = 0.0f;
    _fScale = 0.0f;
    _fFinishPositionX = 0.0f;
    _fFinishPositionY = 0.0f;
    _fEndPositionX = 0.0f;
    _fEndPositionY = 0.0f;
    _iOrderId = 0;
    _fStartScale = 0.0f;
    _fEndScale = 0.0f;
    _fTimer = 0.0f;
    _sIsFilling.clear();
    _fColorR = -1.0f;
    _fColorG = -1.0f;
    _fColorB = -1.0f;
    _sRelation.clear();
    _sType.clear();
    _sRole.clear();
    _sImageTouched.clear();
    _sIsAlpha.clear();
    _iRelationId = 0;
    _iGroupId = 0;
}
void GameSpriteData::setImageId(string sImageId)
{
    _sImageId = sImageId;
}

void GameSpriteData::setSoundId(string sSoundId)
{
    _sSoundId = sSoundId;
}
void GameSpriteData::setTouchEnable(string sTouchEnable)
{
    _sTouchEnable = sTouchEnable;
}

void GameSpriteData::setAnchorPoint(string sAnchorPointX, string sAnchorPointY)
{
    _fAnchorPointX = stringTo<float>(sAnchorPointX);
    _fAnchorPointY = stringTo<float>(sAnchorPointY);
}

void GameSpriteData::setTag(string sTag)
{
    _iTag = stringTo<int>(sTag);
}

void GameSpriteData::setZOrder(string sZOrder)
{
    _iZOrder = stringTo<int>(sZOrder);
}

void GameSpriteData::setOpacity(string sOpacity)
{
    _fOpacity = stringTo<float>(sOpacity);
}

void GameSpriteData::setRotation(string sRotation)
{
    _fRotation = stringTo<float>(sRotation);
}

void GameSpriteData::setScale(string sScale, float resourcesScale)
{
    _fScale = stringTo<float>(sScale)*resourcesScale;
}
void GameSpriteData::setPosition(string sPositionX, string sPositionY ,float coordinateScale,Vec2 winSizeOffset)
{
    _fPositionX = stringTo<float>(sPositionX)*coordinateScale - winSizeOffset.x;
    _fPositionY = stringTo<float>(sPositionY)*coordinateScale - winSizeOffset.y;
    
}
void GameSpriteData::setEndPosition(string sEndPositionX, string sEndPositionY ,float coordinateScale,Vec2 winSizeOffset)
{
    _fEndPositionX = stringTo<float>(sEndPositionX)*coordinateScale - winSizeOffset.x;
    _fEndPositionY = stringTo<float>(sEndPositionY)*coordinateScale - winSizeOffset.y;
}
void GameSpriteData::setIsAlpha(string sIsAlpha)
{
    _sIsAlpha = sIsAlpha;
}

//.......get.......

string GameSpriteData::getImageId()
{
    return _sImageId;
}

string GameSpriteData::getSoundId()
{
    return _sSoundId;
}
string GameSpriteData::getTouchEnable()
{
    return _sTouchEnable;
}
Vec2 GameSpriteData::getPosition()
{
    return Vec2(_fPositionX, _fPositionY);
}
Vec2 GameSpriteData::getAnchorPoint()
{
    return Vec2(_fAnchorPointX, _fAnchorPointY);
}
Vec2 GameSpriteData::getEndPosition()
{
    return Vec2(_fEndPositionX, _fEndPositionY);
}
int GameSpriteData::getTag()
{
    return _iTag;
}
int GameSpriteData::getZOrder()
{
    return _iZOrder;
}
float GameSpriteData::getOpacity()
{
    return _fOpacity;
}
float GameSpriteData::getRotation()
{
    return _fRotation;
}
float GameSpriteData::getScale()
{
    return _fScale;
}

//拼图游戏
void GameSpriteData::setOrderId(string sOrderId)
{
    _iOrderId = stringTo<int>(sOrderId);
}
void GameSpriteData::setStartScale(string sStartScale, float resourcesScale)
{
    _fStartScale = stringTo<float>(sStartScale)*resourcesScale;
}
void GameSpriteData::setEndScale(string sEndScale, float resourcesScale)
{
    _fEndScale = stringTo<float>(sEndScale)*resourcesScale;
}
void GameSpriteData::setFinishPosition(string sFinishPositionX, string sFinishPositionY ,float coordinateScale,Vec2 winSizeOffset)
{
    _fFinishPositionX = stringTo<float>(sFinishPositionX)*coordinateScale - winSizeOffset.x;
    _fFinishPositionY = stringTo<float>(sFinishPositionY)*coordinateScale - winSizeOffset.y;
}
void GameSpriteData::setTimer(string sTimer)
{
    _fTimer = stringTo<float>(sTimer);
}
//.......get........
int GameSpriteData::getOrderId()
{
    return _iOrderId;
}
float GameSpriteData::getStartScale()
{
    return _fStartScale;
}
float GameSpriteData::getEndScale()
{
    return _fEndScale;
}
Vec2 GameSpriteData::getFinishPosition()
{
    return Vec2(_fFinishPositionX, _fFinishPositionY);
}
float GameSpriteData::getTimer()
{
    return _fTimer;
}
//填色游戏
void GameSpriteData::setIsFilling(string sIsFilling)
{
    _sIsFilling = sIsFilling;
}
string GameSpriteData::getIsFilling()
{
    return _sIsFilling;
}
void GameSpriteData::setColor(string sColorR, string sColorG,string sColorB )
{
    _fColorR= stringTo<float>(sColorR);
    _fColorG= stringTo<float>(sColorG);
    _fColorB= stringTo<float>(sColorB);
}
Vec3 GameSpriteData::getColor()
{
    return Vec3(_fColorR, _fColorG, _fColorB);
}
//连线游戏
void GameSpriteData::setRelation(string sRelation)
{
    _sRelation = sRelation;
}
string GameSpriteData::getRelation()
{
    return _sRelation;
}
//顺序连线游戏
void GameSpriteData::setImageTouched(string sImageTouched)
{
    _sImageTouched = sImageTouched;
}
string GameSpriteData::getImageTouched()
{
    return _sImageTouched;
}
//复杂连线游戏
void GameSpriteData::setGroupId(string sGroupId)
{
    _iGroupId = stringTo<int>(sGroupId);
}
int GameSpriteData::getGroupId()
{
    return _iGroupId;
}

//找不同游戏
void GameSpriteData::setType(string sType)
{
    _sType = sType;
}
string GameSpriteData::getType()
{
    return _sType;
}
//新找不同游戏
void GameSpriteData::setRelationId(string sRelationId)
{
    _iRelationId = stringTo<int>(sRelationId);
}
int GameSpriteData::getRelationId()
{
    return _iRelationId;
}
//迷宫游戏
void GameSpriteData::setRole(string sRole)
{
    _sRole = sRole;
}
string GameSpriteData::getRole()
{
    return _sRole;
}
//胜利动画
void GameSpriteData::setIsWin(string sIsWin)
{
    _sIsWin = sIsWin;
}
string GameSpriteData::getIsWin()
{
    return _sIsWin;
}

string GameSpriteData::getIsAlpha()
{
    return _sIsAlpha;
}

NS_FK_END




