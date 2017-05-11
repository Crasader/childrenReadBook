#ifndef FKParticleSystemController_h
#define FKParticleSystemController_h

#include "cocos2d.h"
#include "FKUtil.h"
#include "FKParticleSystemData.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL ParticleSystemController {
public:
    void addParticleSystemForLayer(vector<ParticleSystemData> vParticleSystemData, string sTriggerMode, Layer* layer, string sImageDic, Vec2 touchPosition);
    ParticleSystem* getPaticleSystem(ParticleSystemData particleData, string sImageDic);
};

NS_FK_END
#endif
