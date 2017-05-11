#ifndef KEY_POINT_DATA_H
#define KEY_POINT_DATA_H

#include "FKAnimationData.h"
#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL KeyPointData
{
public:
    KeyPointData();
    ~KeyPointData();
    void                     clear();
    
    void                     setPosition(string sPositionX,string sPositionY,float coordinateScale,Vec2 winSizeOffset);
    void                     setCategory(string sCategory);
    void                     setPointType(string sPointType);
    //终点动画
    void                     setAnimationData(AnimationData &animationData);
    void                     setSpriteTag(string sSpriteTag);    
    void                     setAnimationType(string sAnimationType);
    void                     setTimes(string times);
    
    Vec2                     getPosition();
    int                      getCategory();
    string                   getPointType();
    int                      getSpriteTag();
    string                   getAnimationType();
    vector<AnimationData>    getAnimationData();
    int                      getTimes();
    
private:
    string                   _sTimes;
    float                    _fPositionX;
    float                    _fPositionY;
    int                      _iCategory;
    string                   _sPointType;
    int                      _iSpriteTag;    
    string                   _sAnimationType;    
    vector<AnimationData>    _vAnimationData;
};
NS_FK_END
#endif/* FKKeyPointData_h */
