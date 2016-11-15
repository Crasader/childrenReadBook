#ifndef __ANIMATION_DATA_H__
#define __ANIMATION_DATA_H__

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL AnimationData
{
public:
    
    AnimationData();
    ~AnimationData();
    
    void                        clear();
    
    void                        setAnimationId(string sAnimationId);
    void                        setStyleId(string sStyleId);
    void                        setCategoryId(string sCategoryId);
    void                        setDuration(string sDuration);
    void                        setEndPosition(string sX, string sY ,float coordinateScale,Vec2 winSizeOffset);
    void                        setAngle(string sAngle);
    void                        setScale(string sScale,float resourcesScale);
    void                        setHeight(string sHeight,float coordinateScale);
    void                        setJumps(string sJumps);
    void                        setBlinks(string sBlinks);
    void                        setRGBColor(string sR, string sG, string sB);
    void                        setOpacity(string sOpacity);
    void                        setBezierPoint(string sPointOneX, string sPointOneY, string sPointTwoX, string sPointTwoY ,float coordinateScale,Vec2 winSizeOffset);
    void                        setImageId(string sImageId);
    void                        setSoundId(string sSoundId);
    void                        setFramecount(string sFramecount);
    void                        setPosition(string sPositionX,string sPositionY,float coordinateScale,Vec2 winSizeOffset);
    void                        setScaleXY(string sScaleX,string sScaleY,float resourcesScale);
    void                        setRotateXY(string sAngleX,string sAngleY);
    void                        setTurn(string sTurn);
    void                        setCenter(string sCenterX,string sCenterY,float coordinateScale,Vec2 winSizeOffset);
    void                        setDegrees(string sDegrees);
    void                        setRadius(string sRadius);
    void                        setRate(string sRate);
    void                        setStyle(string sStyle);
    void                        setDelay(string sDelay);
    void                        setCount(string sCount);
    void                        setConstent(string sConstentX,string sConstentY,float coordinateScale);
    
    string                      getAnimationId();
    string                      getStyleId();
    string                      getCategoryId();
    float                       getDuration();
    Vec2                        getEndPosition();
    float                       getAngle();
    float                       getScale();
    float                       getHeight();
    int                         getJumps();
    int                         getBlinks();
    Color3B                     getRGBColor();
    float                       getOpacity();
    Vec2                        getBezierPointOne();
    Vec2                        getBezierPointTwo();
    string                      getImageId();
    string                      getSoundId();
    int                         getFramecount();
    Vec2                        getPosition();
    float                       getScaleX();
    float                       getScaleY();
    float                       getAngleX();
    float                       getAngleY();
    bool                        getTurn();
    Vec2                        getCenter();
    float                       getDegrees();
    float                       getRadius();
    float                       getRate();
    string                      getStyle();
    float                       getDelay();
    int                         getCount();
    int                         getConstentX();
    int                         getConstentY();
    
    
private:
    //styleId用来控制动画类型：move、scale、rotate、jump、blink、tint、fade、delaytime、bezier、scaleXY、rotationXY，place、新增自定义动画
    string                      _sStyleId;
    //categoryId用来控制自动播放动画或者点击播放动画
    string                      _sCategoryId;
    //动画运动时间
    string                      _sDuration;
    //动画id
    string                      _sAnimationId;
    
    //CCMoveTo or CCBezierTo
    //终点坐标
    Vec2                        _stEndPosition;
    
    //CCRotateTo
    //旋转角度
    float                       _fAngle;
    
    //CCScaleTo
    //缩放倍数
    float                       _fScale;
    
    //CCJumpTo
    //跳跃高度
    float                       _fHeight;
    //跳跃次数
    int                         _iJumps;
    
    //CCBlink
    //闪烁次数
    int                         _iBlinks;
    
    //CCTintTo
    //色调RGB
    float                       _fRed;
    float                       _fGreen;
    float                       _fBlue;
    
    //CCFadeTo
    //透明度
    float                       _fOpacity;
    
    //CCBezierTo
    //贝塞尔曲线:控制点P1坐标
    Vec2                        _stControlPoint_1;
    //贝塞尔曲线:控制点P2坐标
    Vec2                        _stControlPoint_2;
    
    //CCAnimation or imageplace(切片替换)
    //帧动画第一帧,imageplace(切片替换)也会用到
    string                      _sImageId;
    //第一帧的音效,imageplace(切片替换)也会用到
    string                      _sSoundId;
    //帧数
    int                         _iFramecount;
    
    //CCPlace
    //放置动画:坐标点
    Vec2                        _stPosition;
    
    //CCScaleTo(X Y)
    //X轴缩放
    float                       _fScaleX;
    //Y轴缩放
    float                       _fScaleY;
    
    //CCRotateTo(X Y)
    //X轴旋转
    float                       _fAngleX;
    //Y轴旋转
    float                       _fAngleY;
    
    //CCDiyRotateBy(源码自定义)
    //布尔值，正向还是反向
    bool                        _bTurn;
    //圆周运动圆心点
    Vec2                        _stCenter;
    //圆周旋转角度
    float                       _fDegrees;
    //圆周运动半径
    float                       _fRadius;
    
    //CCEase
    //速度(新增)
    float                       _fRate;
    string                      _sStyle;
    
    //CCDelayTime
    //延迟时间
    float                       _fDelay;
    
    //customaction or imageplace(切片替换)
    //个数,imageplace(切片替换)也会用到
    int                         _iCount;
    //点动预计X、Y轴的增常量
    int                         _iConstentX;
    int                         _iConstentY;
    
};

NS_FK_END


#endif // __ANIMATION_DATA_H__
