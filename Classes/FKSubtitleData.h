#ifndef __SUBTITLE_DATA_H_
#define __SUBTITLE_DATA_H_

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL SubtitleData
{
public:
    SubtitleData();
    ~SubtitleData();
    
    void clear();
    
    void setImageId(string sImageId);
    string getImageId();
    
    void setPosition(string sPositionX, string sPositionY,float coordinateScale,Vec2 winSizeOffset);
    Vec2 getPosition();
    
    void setZOrder(string sZOrder);
    int getZOrder();
    
    void setStartTime(string sStartTime);
    float getStartTime();
    
    void setDuration(string sDuration);
    float getDuration();
    
    void setAnimationEffect(string sAnimationEffect);
    string getAnimationEffect();
    
private:
    string _sImageId;
    int _iZOrder;
    float _fPositionX;
    float _fPositionY;
    float _fStartTime;
    float _fDuration;
    string _sAnimationEffect;
};

NS_FK_END
#endif /* __SUBTITLE_DATA_H_ */
