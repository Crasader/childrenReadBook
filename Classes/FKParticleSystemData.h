#ifndef __PARTICLE_SYSTEM_DATA_H__
#define __PARTICLE_SYSTEM_DATA_H__

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL ParticleSystemData
{
public:
    ParticleSystemData();
    ~ParticleSystemData();
    void             setCategory(string sCategory);
    void             setParticleStyle(string sParticleStyle);
    void             setImageId(string sImageId);
    void             setPosition(string sPositionX,string sPositionY,float coordinateScale);
    void             setZorder(string sZorder);
    
    string           getCategory();
    string           getParticleStyle();
    string           getImageId();
    Vec2             getPosition();
    int              getZorder();
    
    void             clear();
    
private:
    string          _sCategory;
    string          _sParticleStyle;
    string          _sImageId;
    float           _fPositionX;
    float           _fPositionY;
    int             _iZorder;
    
};

NS_FK_END

#endif/*__PARTICLE_SYSTEM_DATA_H__*/
