#ifndef __NORMAL_POINT_DATA_H__
#define __NORMAL_POINT_DATA_H__

#include "FKAnimationData.h"
#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL NormalPointData
{
public:
    NormalPointData();
    ~NormalPointData();
    void                         clear();
    void                         setNumber(string sNumber);
    void                         setSpriteTag(string sSpriteTag);
    void                         setAnimationType(string sAnimationType);
    void                         setPosition(string sPositionX,string sPositionY,float coordinateScale,Vec2 winSizeOffset);
    void                         setAnimationData(AnimationData &animationData);
    void                         setTimes(string times);

    
    int                          getNumber();
    int                          getSpriteTag();
    string                       getAnimationType();
    Vec2                         getPosition();
    vector<AnimationData>        getAnimationData();
    int                          getTimes();

private:
    string                       _sTimes;
    int                          _iNumber;
    Vec2                         _stPosition;
    int                          _iSpriteTag;
    string                       _sAnimationType;
    vector<AnimationData>        _vAnimationData;
    
    
};

NS_FK_END

#endif /* __NORMAL_POINT_DATA_H__ */
