
#ifndef __GAME_SPRITE_DATA_H__
#define __GAME_SPRITE_DATA_H__

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL GameSpriteData
{
public:
    
    GameSpriteData();
    ~GameSpriteData();
    
    void                clear();
    
    //基本游戏
    void                setImageId(string sImageId);
    void                setSoundId(string sSoundId);
    void                setAnchorPoint(string sAnchorPointX, string sAnchorPointY);
    void                setEndPosition(string sEndPositionX, string sEndPositionY,float coordinateScale,Vec2 winSizeOffset);
    void                setTag(string sTag);
    void                setZOrder(string sZOrder);
    void                setOpacity(string sOpacity);
    void                setRotation(string sRotation);
    void                setScale(string sScale, float resourcesScale);
    void                setPosition(string sPositionX, string sPositionY, float coordinateScale,Vec2 winSizeOffset);
    void                setTouchEnable(string sTouchEnable);
    
    
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
    
    //点击像素检测
    void                setIsAlpha(string sIsAlpha);
    string              getIsAlpha();

    void                setTimer(string sTimer);
    float               getTimer();
    
    //拼图游戏
    //根据isorder判断是否有序，次序ID(顺序连线游戏判断连线次序)
    void                setOrderId(string sOrderId);
    int                 getOrderId();
    
    void                setStartScale(string sStartScale, float resourcesScale);
    void                setEndScale(string sEndScale, float resourcesScale);
    void                setFinishPosition(string sFinishPositionX, string sFinishPositionY ,float coordinateScale,Vec2 winSizeOffset);
    float               getStartScale();
    float               getEndScale();
    Vec2                getFinishPosition();
    
    //填色游戏
    //判断是否是填充颜色区域
    void                setIsFilling(string sIsFilling);
    string              getIsFilling();
    //color RGB参数
    void                setColor(string sColorR,string sColorG,string sColorB);
    Vec3                getColor();
    
    //连线游戏
    //关系，main（主） and minor（从）
    void                setRelation(string sRelation);
    string              getRelation();
    //顺序连线游戏
    void                setImageTouched(string sImageTouched);
    string              getImageTouched();
    //复杂连线游戏
    void                setGroupId(string sGroupId);
    int                 getGroupId();
    
    //找不同游戏
    //游戏切片标志，区分游戏切片和普通切片
    void                setType(string sType);
    string              getType();
    
    //新找不同游戏
    void                setRelationId(string sRelationId);
    int                 getRelationId();
    
    //迷宫游戏主角
    void                setRole(string sRole);
    string              getRole();
    //胜利动画
    void                setIsWin(string sIsWin);
    string              getIsWin();
private:
    //游戏基本属性
    string              _sImageId;
    string              _sSoundId;
    float               _fPositionX;
    float               _fPositionY;
    float               _fAnchorPointX;
    float               _fAnchorPointY;
    float               _fEndPositionX;
    float               _fEndPositionY;
    int                 _iTag;
    int                 _iZOrder;
    float               _fOpacity;
    float               _fRotation;
    float               _fScale;
    float               _fTimer;
    string              _sTouchEnable;
    string              _sIsAlpha;

    //次序ID，拼图/顺序连线
    int                 _iOrderId;
    //拼图游戏
    float               _fFinishPositionX;
    float               _fFinishPositionY;
    float               _fStartScale;
    float               _fEndScale;
    //填色游戏
    string              _sIsFilling;
    float               _fColorR;
    float               _fColorG;
    float               _fColorB;
    //连线游戏
    string              _sRelation;
    //顺序连线游戏
    string              _sImageTouched;
    //复杂连线游戏
    int                 _iGroupId;
    //找不同游戏
    string              _sType;
    //新找不同游戏
    int                 _iRelationId;
    //迷宫游戏
    string              _sRole;
    //胜利动画
    string              _sIsWin;

};

NS_FK_END

#endif /* GameSpriteData_h */
