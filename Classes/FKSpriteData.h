#ifndef __SPRITE_DATA_H__
#define __SPRITE_DATA_H__

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL SpriteData
{
public:
    
    SpriteData();
    ~SpriteData();
    
    
    string              getImageId();
    string              getSoundId();
    Vec2                getPosition();
    Vec2                getAnchorPoint();
    Vec2                getEndPosition();
    int                 getTag();
    int                 getZOrder();
    float               getOpacity();
    float               getRotation();
    float               getScale();
    string              getTouchEnable();
    string              getIsSubtitle();
    string              getIsReplay();
    string              getIsAlpha();
    
    void                setImageId(string sImageId);
    void                setSoundId(string sSoundId);
    void                setCenterPoint(string sCenterPointX, string sCenterPointY,float coordinateScale,Vec2 winSizeOffset);
    void                setAnchorPoint(string sAnchorPointX, string sAnchorPointY);
    void                setEndPosition(string sEndPositionX, string sEndPositionY,float coordinateScale,Vec2 winSizeOffset);
    void                setTag(string sTag);
    void                setZOrder(string sZOrder);
    void                setOpacity(string sOpacity);
    void                setRotation(string sRotation);
    void                setScale(string sScale, float resourcesScale);
    void                setTouchEnable(string sTouchEnable);
    void                setIsSubtitle(string sIsSubtitle);
    void                setIsReplay(string sIsReplay);
    void                setIsAlpha(string sIsAlpha);
    
    void                clear();
private:
    //标准页面sprite基本属性
    string              _sImageId;
    string              _sSoundId;
    float               _fCenterPointX;
    float               _fCenterPointY;
    float               _fAnchorPointX;
    float               _fAnchorPointY;
    float               _fEndPositionX;
    float               _fEndPositionY;
    int                 _iTag;
    int                 _iZOrder;
    float               _fOpacity;
    float               _fRotation;
    float               _fScale;
    string              _sTouchEnable;
    string              _sIsSubtitle;
    string              _sIsReplay;
    string              _sIsAlpha;
};

NS_FK_END

#endif // __SPRITE_DATA_H__
